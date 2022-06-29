
#include "InstancingCBuffer.h"
#include "ConstantBuffer.h"

CInstancingCBuffer::CInstancingCBuffer() :
	m_BufferData{}
{
}

CInstancingCBuffer::CInstancingCBuffer(const CInstancingCBuffer& Buffer) :
	CConstantBufferBase(Buffer)
{
	m_BufferData = Buffer.m_BufferData;
}

CInstancingCBuffer::~CInstancingCBuffer()
{
}

bool CInstancingCBuffer::Init()
{
	SetConstantBuffer("InstancingCBuffer");

	return true;
}

void CInstancingCBuffer::UpdateCBuffer()
{
	m_Buffer->UpdateBuffer(&m_BufferData);
}

CInstancingCBuffer* CInstancingCBuffer::Clone()
{
	return new CInstancingCBuffer(*this);
}
