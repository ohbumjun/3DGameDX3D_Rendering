#include "FirstHDRDownScaleCBuffer.h"
#include "../../Engine.h"
#include "ConstantBuffer.h"

CFirstHDRDownScaleCBuffer::CFirstHDRDownScaleCBuffer() :
	m_BufferData{}
{
}

CFirstHDRDownScaleCBuffer::CFirstHDRDownScaleCBuffer(const CFirstHDRDownScaleCBuffer& Buffer)
{
}

CFirstHDRDownScaleCBuffer::~CFirstHDRDownScaleCBuffer()
{
}

bool CFirstHDRDownScaleCBuffer::Init()
{
	// struct FirstHDRDownScaleCBuffer
	// {
	// 	Vector2 FRes; // 백버퍼의 높이와 너비를 4로 나눈 값  (너비 -> 높이 순서로 )
	// 	int FDomain; // 백퍼퍼와 높이와 너비를 곱한 후 16으로 나눈 값
	// 	int FGroupSize; // 백버퍼의 높이와 너비를 곱한 후, 16으로 나눈 다음 1024를 곱한 값
	// };

	Resolution RS = CEngine::GetInst()->GetResolution();

	m_BufferData.FRes           = Vector2((float)RS.Width, (float)RS.Height) / 4.f;
	m_BufferData.FDomain     = (RS.Width / 4.f) * (RS.Height / 4.f); 
	m_BufferData.FGroupSize  = m_BufferData.FDomain * 1024;

	SetConstantBuffer("FirstHDRDownScaleCBuffer");

	return true;
}

void CFirstHDRDownScaleCBuffer::UpdateCBuffer()
{
	m_Buffer->UpdateBuffer(&m_BufferData);
}

CConstantBufferBase* CFirstHDRDownScaleCBuffer::Clone()
{
	return new CFirstHDRDownScaleCBuffer(*this);
}
