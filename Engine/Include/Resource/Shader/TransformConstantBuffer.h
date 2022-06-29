#pragma once

#include "ConstantBufferBase.h"

class CTransformConstantBuffer :
    public CConstantBufferBase
{
public:
	CTransformConstantBuffer();
	CTransformConstantBuffer(const CTransformConstantBuffer& Buffer);
	virtual ~CTransformConstantBuffer();

protected:
	TransformCBuffer	m_BufferData;

public:
	virtual bool Init();
	virtual void UpdateCBuffer();
	virtual CTransformConstantBuffer* Clone();

public:
	void SetWorldMatrix(const Matrix& matWorld)
	{
		m_BufferData.matWorld = matWorld;
	}

	void SetViewMatrix(const Matrix& matView)
	{
		m_BufferData.matView = matView;
	}

	void SetProjMatrix(const Matrix& matProj)
	{
		m_BufferData.matProj = matProj;
		m_BufferData.matInvProj = matProj;
		m_BufferData.matInvProj.Inverse();
	}

	void SetPivot(const Vector3& Pivot)
	{
		m_BufferData.Pivot = Pivot;
	}

	void SetMeshSize(const Vector3& Size)
	{
		m_BufferData.MeshSize = Size;
	}
};

