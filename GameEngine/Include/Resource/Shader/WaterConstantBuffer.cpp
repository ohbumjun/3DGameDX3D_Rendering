
#include "WaterConstantBuffer.h"
#include "ConstantBuffer.h"

CWaterConstantBuffer::CWaterConstantBuffer() :
	m_BufferData{}
{
	m_BufferData.WaveHeight = 0.5f;
	m_BufferData.WaveSpeed = 2.f;
	m_BufferData.WaveFrequencey = 10.f;
}

CWaterConstantBuffer::CWaterConstantBuffer(const CWaterConstantBuffer& Buffer) :
	CConstantBufferBase(Buffer)
{
	m_BufferData = Buffer.m_BufferData;
}

CWaterConstantBuffer::~CWaterConstantBuffer()
{
}

bool CWaterConstantBuffer::Init()
{
	SetConstantBuffer("WaterCBuffer");

	return true;
}

void CWaterConstantBuffer::UpdateCBuffer()
{
	m_Buffer->UpdateBuffer(&m_BufferData);
}

CWaterConstantBuffer* CWaterConstantBuffer::Clone()
{
	return new CWaterConstantBuffer(*this);
}
