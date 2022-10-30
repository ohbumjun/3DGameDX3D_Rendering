#pragma once

#include "../GameInfo.h"
#include "../Resource/Texture/RenderTarget.h"


class CHDR
{
	friend class CRenderManager;
	// 2개의 버퍼
	// 2개의 상수 버퍼
	// 2개의 컴퓨터 셰이더
protected:
	CHDR();
	CHDR(const CHDR& com) = delete;
	virtual ~CHDR();

public :
	bool Init();
	void RenderFirstDownScale();
	void RenderSecondDownScale();
	void FinalToneMapping();
protected :
	// HDR을 구현하기 위해서는 
	// 1) ShaderResourceView와 UnorderedAccessView를 만들어주기 위한 버퍼2개
	// 2) Input용인 Shader Resource View 2개, Output용으로 사용하는 UnorderedAccessView 2개
	// 3) ToneMapping에 평균 휘도 값을 넣어줄 ShaderResourceView 1개가 필요하다(이하 Shader Resource View 를 SRV, UnorderedAccessView를 UAV라 하겠다)
	class CStructuredBuffer* m_MidDownScaleLumBuffer;     // 휘도값 계산시 중간값 저장하는 역할
	class CStructuredBuffer* m_MeanLumBuffer;

	class CHDRDownScaleFirstPass*		   m_DownScaleFirstPassUpdateShader;
	class CHDRDownScaleSecondPass*    m_DownScaleSecondPassUpdateShader;

	// 상수 버퍼 2개 
	class CFirstHDRDownScaleCBuffer* m_DownScaleCBuffer;
	class CToneMappingCBuffer*         m_ToneMappingCBuffer;

private :
	unsigned int  m_FirstDownScaleThreadGrpCnt;
};

// 원리
// 조명 -> HDR -> (톤 매핑) LDR
// 즉, 셰이더 내에서는 HDR 로 계산을 하고, 렌더링 시에는 LDR 로 적절한 변환을 해야 한다.(톤맵핑)
// 1) 화면의 평균 휘도를 구한다.
// 2) 평균 휘도를 키값으로 하여 표시 가능한 1677만색(RGB8) 으로 매핑한다.




// ID3D11Buffer* m_MidDownScaleBuffer;
// ID3D11UnorderedAccessView* m_MidDownScaleUAV;
// ID3D11ShaderResourceView* m_MidDownScaleSRV;
// 
// ID3D11Buffer* m_FinalLumAvgBuffer;
// ID3D11UnorderedAccessView* m_FinalLumAvgBufferUAV;
// ID3D11ShaderResourceView* m_FinalLumAvgBufferSRV;