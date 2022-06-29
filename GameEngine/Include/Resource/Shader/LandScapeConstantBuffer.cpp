
#include "LandScapeConstantBuffer.h"
#include "ConstantBuffer.h"

CLandScapeConstantBuffer::CLandScapeConstantBuffer() :
	m_BufferData{}
{
	m_BufferData.DetailLevel = 1.f;
}

CLandScapeConstantBuffer::CLandScapeConstantBuffer(const CLandScapeConstantBuffer& Buffer) :
	CConstantBufferBase(Buffer)
{
	m_BufferData = Buffer.m_BufferData;
}

CLandScapeConstantBuffer::~CLandScapeConstantBuffer()
{
}

bool CLandScapeConstantBuffer::Init()
{
	SetConstantBuffer("LandScapeCBuffer");

	return true;
}

void CLandScapeConstantBuffer::UpdateCBuffer()
{
	m_Buffer->UpdateBuffer(&m_BufferData);
}

CLandScapeConstantBuffer* CLandScapeConstantBuffer::Clone()
{
	return new CLandScapeConstantBuffer(*this);
}
