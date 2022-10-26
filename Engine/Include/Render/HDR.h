#pragma once

#include "../GameInfo.h"
#include "../Resource/Texture/RenderTarget.h"

class CHDR
{
	// 2개의 버퍼
	// 2개의 상수 버퍼
	// 2개의 컴퓨터 셰이더
protected:
	CHDR();
	CHDR(const CHDR& com) = delete;
	virtual ~CHDR();

public :
	bool Init();

protected :
	class CStructuredBuffer* m_MiddleLumBuffer;     // 휘도값 계산시 중간값 저장하는 역할
	class CStructuredBuffer* m_MeanLumBuffer;

	class CHDRDownScaleFirstPass*		   m_FirstPassUpdateShader;
	class CHDRDownScaleSecondPass*    m_SecondPassUpdateShader;

	// 상수 버퍼 2개 
	class CFirstHDRDownScaleCBuffer* m_DownScaleCBuffer;

	// 조명 -> HDR -> (톤 매핑) LDR
};

