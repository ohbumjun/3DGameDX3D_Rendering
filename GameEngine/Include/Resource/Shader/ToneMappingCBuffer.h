#pragma once
#include "ConstantBufferBase.h"

class CToneMappingCBuffer :
    public CConstantBufferBase
{
public:
	CToneMappingCBuffer();
	CToneMappingCBuffer(const CToneMappingCBuffer& Buffer);
	virtual ~CToneMappingCBuffer();

protected:
	ToneMappingCBuffer	m_BufferData;

public:
	virtual bool Init();
	virtual void UpdateCBuffer();
	virtual CConstantBufferBase* Clone();
};

