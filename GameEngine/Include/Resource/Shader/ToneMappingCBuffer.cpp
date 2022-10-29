#include "ToneMappingCBuffer.h"
#include "ConstantBuffer.h"

CToneMappingCBuffer::CToneMappingCBuffer()
{
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
