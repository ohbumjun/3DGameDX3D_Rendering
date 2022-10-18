#pragma once
#include "ConstantBufferBase.h"

class CLightFowardConstantBuffer :
    public CConstantBufferBase
{
public:
	CLightFowardConstantBuffer();
	CLightFowardConstantBuffer(const CLightFowardConstantBuffer& Buffer);
	virtual ~CLightFowardConstantBuffer();

protected:
	LightFowardCBuffer	 m_BufferData;

public:
	virtual bool Init();
	virtual void UpdateCBuffer();
	virtual CLightFowardConstantBuffer* Clone();

public:
	void SetLightCount(int LightCount)
	{
		m_BufferData.LightCount = LightCount;
	}
};

