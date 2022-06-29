
#include "LightConstantBuffer.h"
#include "ConstantBuffer.h"

CLightConstantBuffer::CLightConstantBuffer() :
	m_BufferData{}
{
	m_BufferData.Color = Vector4(1.f, 1.f, 1.f, 1.f);
	m_BufferData.Distance = 10.f;
	m_BufferData.AngleIn = 10.f;
	m_BufferData.AngleOut = 15.f;
	m_BufferData.Att1 = 0.f;
	m_BufferData.Att2 = 0.f;
	m_BufferData.Att3 = 1.f;
}

CLightConstantBuffer::CLightConstantBuffer(const CLightConstantBuffer& Buffer) :
	CConstantBufferBase(Buffer)
{
	m_BufferData = Buffer.m_BufferData;
}

CLightConstantBuffer::~CLightConstantBuffer()
{
}

bool CLightConstantBuffer::Init()
{
	SetConstantBuffer("LightCBuffer");

	return true;
}

void CLightConstantBuffer::UpdateCBuffer()
{
	m_Buffer->UpdateBuffer(&m_BufferData);
}

CLightConstantBuffer* CLightConstantBuffer::Clone()
{
	return new CLightConstantBuffer(*this);
}
