#include "HDR.h"
#include "../Shader/StructuredBuffer.h"
#include "../Shader/FirstHDRDownScaleCBuffer.h"
#include "../Resource/ResourceManager.h"
#include "../Shader/FirstHDRDownScaleCBuffer.h"

CHDR::CHDR()
{
}

CHDR::~CHDR()
{
	SAFE_DELETE(m_MiddleLumBuffer);
	SAFE_DELETE(m_MeanLumBuffer);

	SAFE_DELETE(m_MeanLumBuffer);
}

bool CHDR::Init()
{


	// First Pass 에서는 전체 화면의 픽셀들을 Group 으로 나눠서 Down Scale 할 때 사용한다.
	m_MiddleLumBuffer = new CStructuredBuffer;
	
	// Count * sizeof(float) == 4 * Total Back Buffer Pixel  / (16 * 1024);
	int Count = 1280 * 720 / (16 * 1024);

	if (!m_MiddleLumBuffer->Init("MiddleLumBuffer", sizeof(float), Count, 35, false, (int)Buffer_Shader_Type::Compute))
	{
		SAFE_DELETE(m_MiddleLumBuffer);
		assert(false);
		return false;
	}

	m_MeanLumBuffer = new CStructuredBuffer;

	// Second Pass 에서는 FirstPass 에서 나온 float 값 하나를 넣어준다.
	if (!m_MeanLumBuffer->Init("MeanLumBuffer", sizeof(float), 1, 36, false, (int)Buffer_Shader_Type::Compute))
	{
		SAFE_DELETE(m_MeanLumBuffer);
		assert(false);
		return false;
	}

	m_FirstPassUpdateShader = (CHDRDownScaleFirstPass*)CResourceManager::GetInst()->FindShader("HDRDownScaleFirstPass");
	m_SecondPassUpdateShader = (CHDRDownScaleSecondPass*)CResourceManager::GetInst()->FindShader("HDRDownScaleSecondPass");

	m_DownScaleCBuffer = new CFirstHDRDownScaleCBuffer;

	if (!m_DownScaleCBuffer->Init())
		return false;

	return true;
}

void CHDR::RenderSetting()
{
}
