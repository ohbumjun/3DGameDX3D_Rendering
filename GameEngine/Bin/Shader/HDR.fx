#include "ShaderInfo.fx"


cbuffer FirstHDRDownScaleCBuffer : register(b7)
{
	float2 g_Res;  // 백버퍼의 높이와 너비를 4로 나눈 값  (너비 -> 높이 순서로 )
	uint   g_Domain;  // 백퍼퍼와 높이와 너비를 곱한 후 16으로 나눈 값
	uint   g_GroupSize; // 백버퍼의 높이와 너비를 곱한 후, 16으로 나눈 다음 1024를 곱한 값
};

// Render Target 만들어준 것 넘겨줘야 한다.
Texture2D HDRTex : register(t81);

StructuredBuffer<float>		    AverageValues1DSRV	: register(t35);// 읽기 전용
RWStructuredBuffer<float>    AverageLumFinalUAV : register(u36);  // 읽기, 쓰기 둘다 가능

// 휘도 계산을 위한 상수 
static const float4 LUM_FACTOR = float4(0.299, 0.587, 0.114, 0);

// 1번째 Pass
// 그룹 공유 메모리  (같은 그룹 내 쓰레드끼리만 에만 공유되는 정보 ==> 레지스터 ~ 장치 메모리 사이에 위치하여 읽기 쓰기 속도 및 용량이 중간 정도)
// 총 1024개의 쓰레드가 일을 하고 있는 상태
groupshared float SharedPositions[1024];

// First Pass 에서는 3번의 Down Scale 이 일어난다.
// 1) 16 픽셀 그룹은 하나의 픽셀로 줄인다.
// 2) 1024에서 4로 Down Scale
// 3) 4 에서 1 로 Down Scale

// 각 스레드에 대해 4x4 다운 스케일을 수행한다
float DownScale4x4(uint2 CurPixel, uint groupThreadId)
{
	// groupThreadId : 한 쓰레드 그룹 내에서의 Idx

	float avgLum = 0.f;

	// 픽셀 결합 생략
	if (CurPixel.y < g_Res.y)
	{
		int3 iFullResPos = int3(CurPixel * 4, 0);
		float4 vDownScaled = float4(0.f, 0.f, 0.f, 0.f);

		[unroll]
		for (int i = 0; i < 4; ++i)
		{
			[unroll]
			for (int j = 0; j < 4; ++j)
			{
				vDownScaled += HDRTex.Load(iFullResPos, int2(j, i));
			}
		}

		vDownScaled /= 16;

		// 픽셀별 휘도 값 계산
		avgLum = dot(vDownScaled, LUM_FACTOR);

		// 공유 메모리에 결과 기록
		SharedPositions[groupThreadId] = avgLum;
	}

	// 동기화 후 다음 단계로 (스레드 그룹 내에 모든 스레드가 해당 함수에 도달할 때까지 그룹 내에 모든 쓰레드의 실행을 블로킹 한다.)
	GroupMemoryBarrierWithGroupSync();

	return avgLum;
}


// 위에서 구한 값을 4개의 값으로 다운스케일한다
float DownScale1024to4(uint dispachThreadId, uint groupThreadId,
	float avgLum)
{
	[unroll]
	for (uint iGroupSize = 4, iStep1 = 1, iStep2 = 2, iStep3 = 3;
		iGroupSize < 1024;
		iGroupSize *= 4, iStep1 *= 4, iStep2 *= 4, iStep3 *= 4)
	{
		if (groupThreadId % iGroupSize == 0)
		{
			float fStepAvgLum = avgLum;

			fStepAvgLum += dispachThreadId + iStep1 < g_Domain ?
				SharedPositions[groupThreadId + iStep1] : avgLum;

			fStepAvgLum += dispachThreadId + iStep2 < g_Domain ?
				SharedPositions[groupThreadId + iStep2] : avgLum;

			fStepAvgLum += dispachThreadId + iStep3 < g_Domain ?
				SharedPositions[groupThreadId + iStep3] : avgLum;

			// 결과 값 저장
			avgLum = fStepAvgLum;
			SharedPositions[groupThreadId] = fStepAvgLum;
		}

		// 동기화 후 다음으로
		GroupMemoryBarrierWithGroupSync();
	}

	return avgLum;
}

void DownScale4to1(uint dispatchThreadId, uint groupThreadId,
	uint groupId, float avgLum)
{
	if (groupThreadId == 0)
	{
		//  스레드 그룹에 대한 평균 휘도 값 계산
		float fFinalAvgLum = avgLum;

		fFinalAvgLum += dispatchThreadId + 256 < g_Domain ?
			SharedPositions[groupThreadId + 256] : avgLum;

		fFinalAvgLum += dispatchThreadId + 512 < g_Domain ?
			SharedPositions[groupThreadId + 512] : avgLum;

		fFinalAvgLum += dispatchThreadId + 768 < g_Domain ?
			SharedPositions[groupThreadId + 768] : avgLum;

		fFinalAvgLum /= 1024.f;

		// 최종 값을 ID UAV에 기록 후 다음 과정으로
		AverageLumFinalUAV[groupId] = fFinalAvgLum;
	}
}

// 이렇게 구한 값은 셰이더 엔트리 포인트에 대입된다 -> 
[numthreads(1024, 1, 1)]
void DownScaleFirstPass(uint3 groupId : SV_GroupID, // dispatch 호출의 전체 쓰레드 그룹 중에서, 현재 스레드가 속한 그룹 3차원 식별자 
	uint3 dispatchThreadId : SV_DispatchThreadID,      // 전체 dispatch 안에서의 현재 쓰레드의 3차원 식별자 (쓰레드의 고유 ID 라고 할 수 있다)
	uint3 groupThreadId : SV_GroupThreadID)             // 현재 스레드가 속한 스레드 그룹 안에서의 Idx
{
	uint2 vCurPixel = uint2(dispatchThreadId.x % g_Res.x,
		dispatchThreadId.x / g_Res.x);

	// 16 픽셀 그룹을 하나의 픽셀로 줄여 공유 메모리에 저장
	float favgLum = DownScale4x4(vCurPixel, groupThreadId.x);

	// 1024에서 4로 다운스케일
	favgLum = DownScale1024to4(dispatchThreadId.x, groupThreadId.x,
		favgLum);

	// 4에서 1로 다운스케일
	DownScale4to1(dispatchThreadId.x, groupThreadId.x, groupId.x,
		favgLum);

	// 이 컴퓨트 셰이더는 x 값 (백버퍼의 총 픽셀 수 / (16 * 1024)) 에 따라 묶어서 처리할 수 있다
}

// SecondPass에서도 3번의 DownScale이 일어나는데
// 1. 64에서 16으로 DownScale
// 2. 16에서 4로 DownScale
// 3. 4에서 1로 DownScale

// 참고 : HDR 의 모든 과정은 선형 공간에서 처리되어야 한다.
#define MAX_GROUPS 64

// 2번째 Pass
// 공유 메모리 그룹에 중간 값 저장
groupshared float SharedAvgFinal[MAX_GROUPS];

/// 첫 번째 컴퓨트 셰이더의 실행이 완료되면 동일한 상수 버퍼를 사용한 두번째 컴퓨트 셰이더를 실행한다
// 중간 값 휘도 SRV와 평균 휘도 UAV 값을 지정해 사용한다
// 최종적으로 평균 휘도값을 구해서 사용할 것이다.
[numthreads(MAX_GROUPS, 1, 1)]
void DownScaleSecondPass(uint3 groupId : SV_GroupID,
	uint3 groupThreadId : SV_GroupThreadID,
	uint3 dispatchThreadId : SV_DispatchThreadID)
{
	// 공유 메모리에 ID값 저장
	float favgLum = 0.f;

	if (dispatchThreadId.x < g_GroupSize)
	{
		favgLum = AverageValues1DSRV[dispatchThreadId.x];
	}

	SharedAvgFinal[dispatchThreadId.x] = favgLum;

	GroupMemoryBarrierWithGroupSync(); // 동기화 후 다음 과정으로

	// 64에서 16으로 다운 스케일
	if (dispatchThreadId.x % 4 == 0)
	{
		// 휘도 값 합산
		float fstepAvgLum = favgLum;

		fstepAvgLum += dispatchThreadId.x + 1 < g_GroupSize ?
			SharedAvgFinal[dispatchThreadId.x + 1] : favgLum;

		fstepAvgLum += dispatchThreadId.x + 2 < g_GroupSize ?
			SharedAvgFinal[dispatchThreadId.x + 2] : favgLum;

		fstepAvgLum += dispatchThreadId.x + 3 < g_GroupSize ?
			SharedAvgFinal[dispatchThreadId.x + 3] : favgLum;

		// 결과 값 저장
		favgLum = fstepAvgLum;

		SharedAvgFinal[dispatchThreadId.x] = fstepAvgLum;
	}

	GroupMemoryBarrierWithGroupSync(); // 동기화 후 다음 과정으로

	// 4에서 1로 다운스케일
	if (dispatchThreadId.x == 0)
	{
		// 휘도 값 합산
		float fFinalLumValue = favgLum;

		fFinalLumValue += dispatchThreadId.x + 16 < g_GroupSize ?
			SharedAvgFinal[dispatchThreadId.x + 16] : favgLum;

		fFinalLumValue += dispatchThreadId.x + 32 < g_GroupSize ?
			SharedAvgFinal[dispatchThreadId.x + 32] : favgLum;

		fFinalLumValue += dispatchThreadId.x + 48 < g_GroupSize ?
			SharedAvgFinal[dispatchThreadId.x + 48] : favgLum;

		fFinalLumValue /= 64.f;

		AverageLumFinalUAV[0] = max(fFinalLumValue, 0.0001);
	}
}