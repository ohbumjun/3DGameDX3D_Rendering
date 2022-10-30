#include "ToneMappingCBuffer.h"
#include "ConstantBuffer.h"

CToneMappingCBuffer::CToneMappingCBuffer()
{
	m_BufferData.MiddleGrey = 2;
	m_BufferData.LumWhiteSqr = 30;
}

CToneMappingCBuffer::CToneMappingCBuffer(const CToneMappingCBuffer& Buffer)
{
}

CToneMappingCBuffer::~CToneMappingCBuffer()
{
}

bool CToneMappingCBuffer::Init()
{
	SetConstantBuffer("ToneMappingCBuffer");

	return true;
}

void CToneMappingCBuffer::UpdateCBuffer()
{
	m_Buffer->UpdateBuffer(&m_BufferData);
}

CConstantBufferBase* CToneMappingCBuffer::Clone()
{
	return new CToneMappingCBuffer(*this);
}
