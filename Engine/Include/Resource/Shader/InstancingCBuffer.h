#pragma once
#include "ConstantBufferBase.h"
class CInstancingCBuffer :
    public CConstantBufferBase
{
public:
	CInstancingCBuffer();
	CInstancingCBuffer(const CInstancingCBuffer& Buffer);
	virtual ~CInstancingCBuffer();

protected:
	InstancingCBuffer	m_BufferData;

public:
	virtual bool Init();
	virtual void UpdateCBuffer();
	virtual CInstancingCBuffer* Clone();

public:
	void SetBoneCount(int BoneCount)
	{
		m_BufferData.InstancingBoneCount = BoneCount;
	}
};

