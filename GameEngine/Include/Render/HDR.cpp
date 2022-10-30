#include "HDR.h"
#include "../Shader/StructuredBuffer.h"
#include "../Shader/ToneMappingCBuffer.h"
#include "../Shader/FirstHDRDownScaleCBuffer.h"
#include "../Resource/ResourceManager.h"
#include "../Shader/HDRDownScaleFirstPass.h"
#include "../Shader/HDRDownScaleSecondPass.h"
#include "../Engine.h"
#include "../Device.h"

CHDR::CHDR()
{
}

CHDR::~CHDR()
{
	SAFE_DELETE(m_MidDownScaleLumBuffer);
	SAFE_DELETE(m_MeanLumBuffer);
	SAFE_DELETE(m_DownScaleCBuffer);
	SAFE_DELETE(m_ToneMappingCBuffer);
}

bool CHDR::Init()
{
	// First Pass 에서는 전체 화면의 픽셀들을 Group 으로 나눠서 Down Scale 할 때 사용한다.
	// 중간 휘도 (휘도값을 계산할 때 중간값을 저장하는 역할)
	m_MidDownScaleLumBuffer = new CStructuredBuffer;
	
	// Count * sizeof(float) == 4 * Total Back Buffer Pixel  / (16 * 1024);
	// int Count = 1280 * 720 / (16 * 1024);
	Resolution RS = CEngine::GetInst()->GetResolution();
	
	unsigned int TotalPixelCnt = RS.Width * RS.Height;
	m_FirstDownScaleThreadGrpCnt = (unsigned int)ceil((TotalPixelCnt / 16) / 1024);

	if (!m_MidDownScaleLumBuffer->Init("MiddleLumBuffer", sizeof(float),
		m_FirstDownScaleThreadGrpCnt, 5, false, (int)Buffer_Shader_Type::Compute))
	{
		SAFE_DELETE(m_MidDownScaleLumBuffer);
		assert(false);
		return false;
	}

	// 부동소수점 형태로 평균 휘도값 저장 
	m_MeanLumBuffer = new CStructuredBuffer;
	
	// Second Pass 에서는 FirstPass 에서 나온 float 값 하나를 넣어준다.
	if (!m_MeanLumBuffer->Init("MeanLumBuffer", sizeof(float), 1, 
		6, false, (int)Buffer_Shader_Type::All))
	{
		SAFE_DELETE(m_MeanLumBuffer);
		assert(false);
		return false;
	}

	m_DownScaleFirstPassUpdateShader = (CHDRDownScaleFirstPass*)CResourceManager::GetInst()->FindShader("HDRDownScaleFirstPass");
	m_DownScaleSecondPassUpdateShader = (CHDRDownScaleSecondPass*)CResourceManager::GetInst()->FindShader("HDRDownScaleSecondPass");

	m_DownScaleCBuffer = new CFirstHDRDownScaleCBuffer;

	if (!m_DownScaleCBuffer->Init())
		return false;

	m_ToneMappingCBuffer = new CToneMappingCBuffer;

	if (!m_ToneMappingCBuffer->Init())
		return false;

	return true;
}

// First Pass 에서는 3번의 Down Scale 이 일어난다.
// 1) 16 픽셀 그룹은 하나의 픽셀로 줄인다. (쓰레드 그룹 하나에는 1024개의 쓰레드, 각 쓰레드가 16개의 pixeel 을 처리)
// ex) 1280 x 720 의 해상도를 처리하기 위해서는 / (16 * 1024) => 56.25 개의 쓰레드가 존재해야 한다.
// 2) 1024에서 4로 Down Scale
// 3) 4 에서 1 로 Down Scale (4개의 값을 하나의 평균값으로 다운스케일)
void CHDR::RenderFirstDownScale()
{
	m_DownScaleCBuffer->UpdateCBuffer();

	// 쓰기 전용으로 넘겨준다.
	m_MidDownScaleLumBuffer->SetShader();

	m_DownScaleFirstPassUpdateShader->Excute(m_FirstDownScaleThreadGrpCnt, 1, 1);

	// 혹시 모르니 나중에 빼주자 (Second Down Scale 에서)
	m_MidDownScaleLumBuffer->ResetShader();
}

// SecondPass에서도 3번의 DownScale이 일어나는데
// 1. 64에서 16으로 DownScale
// 2. 16에서 4로 DownScale
// 3. 4에서 1로 DownScale
// -> 최종적으로 평균 휘도 값을 구할 수 있다.
void CHDR::RenderSecondDownScale()
{
	// First Pass 에서 해준 상태 
	// m_DownScaleCBuffer->UpdateCBuffer();

	// 쓰기 전용
	m_MeanLumBuffer->SetShader();

	// 중간 휘도 SRV : 읽기 전용으로 넘겨준다.
	m_MidDownScaleLumBuffer->SetShader(15, (int)Buffer_Shader_Type::Compute);

	// 평균 휘도 UAV
	// 2번째에서는 1개의 쓰레드 그룹만 사용하면 된다.
	m_DownScaleSecondPassUpdateShader->Excute(1, 1, 1);

	m_MidDownScaleLumBuffer->ResetShader(15, (int)Buffer_Shader_Type::Compute);

	// 쓰기 전용
	m_MeanLumBuffer->ResetShader();

}

void CHDR::FinalToneMapping()
{
	m_ToneMappingCBuffer->UpdateCBuffer();

	// 최종 SRV : 읽기 전용으로 넘겨준다.
	m_MeanLumBuffer->SetShader(15, (int)Buffer_Shader_Type::Graphic);

	// Null Buffer 출력
	UINT Offset = 0;
	CDevice::GetInst()->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	CDevice::GetInst()->GetContext()->IASetVertexBuffers(0, 0, nullptr, nullptr, &Offset);
	CDevice::GetInst()->GetContext()->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

	CDevice::GetInst()->GetContext()->Draw(4, 0);

	m_MeanLumBuffer->ResetShader(15, (int)Buffer_Shader_Type::Graphic);

}



