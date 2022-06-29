#pragma once
#include "ConstantBufferBase.h"
class CLandScapeConstantBuffer :
    public CConstantBufferBase
{
public:
	CLandScapeConstantBuffer();
	CLandScapeConstantBuffer(const CLandScapeConstantBuffer& Buffer);
	virtual ~CLandScapeConstantBuffer();

protected:
	LandScapeCBuffer	m_BufferData;

public:
	virtual bool Init();
	virtual void UpdateCBuffer();
	virtual CLandScapeConstantBuffer* Clone();

public:
	void SetDetailLevel(float Level)
	{
		m_BufferData.DetailLevel = Level;
	}

	void SetSplatCount(int Count)
	{
		m_BufferData.SplatCount = Count;
	}
};

