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

protected :
	class CStructuredBuffer* m_FirstDownScaleShaderBuffer;
	class CStructuredBuffer* m_SecondDownScaleShaderBuffer;

	CSharedPtr<CRenderTarget>	m_MidLuminanceTarget;
	CSharedPtr<CRenderTarget>	m_AvgLuminanceTarget;
};

