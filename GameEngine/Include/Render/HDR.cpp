#include "HDR.h"
#include "../Shader/StructuredBuffer.h"
#include "../Shader/FirstHDRDownScaleCBuffer.h"
#include "../Resource/ResourceManager.h"
#include "../Shader/HDRDownScaleFirstPass.h"
#include "../Shader/HDRDownScaleSecondPass.h"

CHDR::CHDR()
{
}

CHDR::~CHDR()
{
	SAFE_DELETE(m_MiddleLumBuffer);
	SAFE_DELETE(m_MeanLumBuffer);
	SAFE_DELETE(m_DownScaleCBuffer);
}

bool CHDR::Init()
{
	// First Pass 에서는 전체 화면의 픽셀들을 Group 으로 나눠서 Down Scale 할 때 사용한다.
	// 중간 휘도
	m_MiddleLumBuffer = new CStructuredBuffer;
	
	// Count * sizeof(float) == 4 * Total Back Buffer Pixel  / (16 * 1024);
	int Count = 1280 * 720 / (16 * 1024);

	if (!m_MiddleLumBuffer->Init("MiddleLumBuffer", sizeof(float), Count, 5, false, (int)Buffer_Shader_Type::Compute))
	{
		SAFE_DELETE(m_MiddleLumBuffer);
		assert(false);
		return false;
	}

	// 평균 휘도
	// m_MeanLumBuffer = new CStructuredBuffer;
	// 
	// // Second Pass 에서는 FirstPass 에서 나온 float 값 하나를 넣어준다.
	// if (!m_MeanLumBuffer->Init("MeanLumBuffer", sizeof(float), 1, 36, false, (int)Buffer_Shader_Type::Compute))
	// {
	// 	SAFE_DELETE(m_MeanLumBuffer);
	// 	assert(false);
	// 	return false;
	// }

	m_FirstPassUpdateShader = (CHDRDownScaleFirstPass*)CResourceManager::GetInst()->FindShader("HDRDownScaleFirstPass");
	m_SecondPassUpdateShader = (CHDRDownScaleSecondPass*)CResourceManager::GetInst()->FindShader("HDRDownScaleSecondPass");

	m_DownScaleCBuffer = new CFirstHDRDownScaleCBuffer;

	if (!m_DownScaleCBuffer->Init())
		return false;

	return true;
}

// First Pass 에서는 3번의 Down Scale 이 일어난다.
// 1) 16 픽셀 그룹은 하나의 픽셀로 줄인다.
// 2) 1024에서 4로 Down Scale
// 3) 4 에서 1 로 Down Scale (4개의 값을 하나의 평균값으로 다운스케일)
void CHDR::RenderFirstDownScale()
{
	m_DownScaleCBuffer->UpdateCBuffer();

	// 쓰기 전용으로 넘겨준다.
	m_MiddleLumBuffer->SetShader();

	m_FirstPassUpdateShader->Excute(1, 1, 1);

	m_MiddleLumBuffer->ResetShader();
}

// SecondPass에서도 3번의 DownScale이 일어나는데
// 1. 64에서 16으로 DownScale
// 2. 16에서 4로 DownScale
// 3. 4에서 1로 DownScale
// -> 최종적으로 평균 휘도 값을 구할 수 있다.
void CHDR::RenderSecondDownScale()
{
	m_DownScaleCBuffer->UpdateCBuffer();

	// 중간 휘도 SRV : 읽기 전용으로 넘겨준다.
	m_MiddleLumBuffer->SetShader(15, (int)Buffer_Shader_Type::Compute);

	// 평균 휘도 UAV
	// m_MeanLumBuffer->SetShader();

	m_SecondPassUpdateShader->Excute(1, 1, 1);

	m_MiddleLumBuffer->ResetShader(15, (int)Buffer_Shader_Type::Compute);

	// m_MeanLumBuffer->ResetShader();
}



