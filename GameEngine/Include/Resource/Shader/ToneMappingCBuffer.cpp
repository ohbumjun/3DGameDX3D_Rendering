#include "ToneMappingCBuffer.h"
#include "ConstantBuffer.h"

CToneMappingCBuffer::CToneMappingCBuffer()
{
	m_BufferData.MiddleGrey = 1.f;
	m_BufferData.LumWhiteSqr = 1.f;
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
