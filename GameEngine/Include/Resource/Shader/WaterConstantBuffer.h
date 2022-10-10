#pragma once
#include "ConstantBufferBase.h"

class CWaterConstantBuffer :
    public CConstantBufferBase
{
public:
	CWaterConstantBuffer();
	CWaterConstantBuffer(const CWaterConstantBuffer& Buffer);
	virtual ~CWaterConstantBuffer();

protected:
	WaterCBuffer	m_BufferData;

public:
	virtual bool Init();
	virtual void UpdateCBuffer();
	virtual CWaterConstantBuffer* Clone();

public:
	void SetWaveHeight(float WaveHeight)
	{
		m_BufferData.WaveHeight = WaveHeight;
	}
	void SetWaveSpeed(float WaveSpeed)
	{
		m_BufferData.WaveSpeed = WaveSpeed;
	}
	void SetWaveFrequency(float WaveFrequencey)
	{
		m_BufferData.WaveFrequencey = WaveFrequencey;
	}
};

