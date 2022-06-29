
#include "ShadowCBuffer.h"
#include "ConstantBuffer.h"

CShadowCBuffer::CShadowCBuffer() :
	m_BufferData{}
{
	m_BufferData.ShadowBias = 0.05f;
}

CShadowCBuffer::CShadowCBuffer(const CShadowCBuffer& Buffer) :
	CConstantBufferBase(Buffer)
{
	m_BufferData = Buffer.m_BufferData;
}

CShadowCBuffer::~CShadowCBuffer()
{
}

bool CShadowCBuffer::Init()
{
	SetConstantBuffer("ShadowCBuffer");

	return true;
}

void CShadowCBuffer::UpdateCBuffer()
{
	m_BufferData.ShadowResolution.x = SHADOWMAP_WIDTH;
	m_BufferData.ShadowResolution.y = SHADOWMAP_HEIGHT;

	m_Buffer->UpdateBuffer(&m_BufferData);
}

CShadowCBuffer* CShadowCBuffer::Clone()
{
	return new CShadowCBuffer(*this);
}
