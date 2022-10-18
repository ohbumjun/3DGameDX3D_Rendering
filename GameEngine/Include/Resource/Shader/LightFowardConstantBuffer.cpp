#include "LightFowardConstantBuffer.h"
#include "ConstantBuffer.h"

CLightFowardConstantBuffer::CLightFowardConstantBuffer() :
	m_BufferData{}
{
}

CLightFowardConstantBuffer::CLightFowardConstantBuffer(const CLightFowardConstantBuffer& Buffer) :
	CConstantBufferBase(Buffer)
{
	m_BufferData = Buffer.m_BufferData;
}

CLightFowardConstantBuffer::~CLightFowardConstantBuffer()
{
}

bool CLightFowardConstantBuffer::Init()
{
	SetConstantBuffer("LightFowardCBuffer");

	return true;
}

void CLightFowardConstantBuffer::UpdateCBuffer()
{
	m_Buffer->UpdateBuffer(&m_BufferData);
}

CLightFowardConstantBuffer* CLightFowardConstantBuffer::Clone()
{
	return new CLightFowardConstantBuffer(*this);
}
