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
public :
	float GetAdaptValue()
	{
		return m_BufferData.FAdaptation;
	}
public :
	void SetAdaptValue(float Value)
	{
		m_BufferData.FAdaptation = Value;
	}
};

