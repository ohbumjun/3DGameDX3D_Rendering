#pragma once
#include "ConstantBufferBase.h"

class CFirstHDRDownScaleCBuffer :
    public CConstantBufferBase
{
public:
	CFirstHDRDownScaleCBuffer();
	CFirstHDRDownScaleCBuffer(const CFirstHDRDownScaleCBuffer& Buffer);
	virtual ~CFirstHDRDownScaleCBuffer();

protected:
	FirstHDRDownScaleCBuffer	m_BufferData;

public:
	virtual bool Init();
	virtual void UpdateCBuffer();
	virtual CConstantBufferBase* Clone();

};

