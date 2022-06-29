#pragma once

#include "ConstantBufferBase.h"

struct ShadowCBuffer
{
	Matrix	matShadowVP;
	Matrix	matShadowInvVP;
	float	ShadowBias;
	Vector2	ShadowResolution;
	float	Empty;
};

class CShadowCBuffer :
    public CConstantBufferBase
{
public:
	CShadowCBuffer();
	CShadowCBuffer(const CShadowCBuffer& Buffer);
	virtual ~CShadowCBuffer();

protected:
	ShadowCBuffer	m_BufferData;

public:
	virtual bool Init();
	virtual void UpdateCBuffer();
	virtual CShadowCBuffer* Clone();

public:
	void SetShadowVP(Matrix matVP)
	{
		m_BufferData.matShadowVP = matVP;
		m_BufferData.matShadowInvVP = matVP;

		m_BufferData.matShadowInvVP.Inverse();

		m_BufferData.matShadowVP.Transpose();
		m_BufferData.matShadowInvVP.Transpose();
	}

	void SetBias(float Bias)
	{
		m_BufferData.ShadowBias = Bias;
	}
};

