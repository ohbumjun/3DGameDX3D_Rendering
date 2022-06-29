#pragma once

#include "../../GameInfo.h"

class CSkeletonSocket
{
	friend class CSkeleton;
	friend class CAnimationSequenceInstance;

private:
	CSkeletonSocket();
	CSkeletonSocket(const CSkeletonSocket& Socket);
	~CSkeletonSocket();

private:
	std::string		m_Name;
	std::string		m_BoneName;
	int				m_BoneIndex;
	Vector3			m_Offset;
	Vector3			m_OffsetRot;
	Matrix			m_matSocket;

public:
	const std::string& GetName()	const
	{
		return m_Name;
	}

	const Matrix& GetSocketMatrix()	const
	{
		return m_matSocket;
	}

public:
	void SetOffset(float x, float y, float z)
	{
		m_Offset = Vector3(x, y, z);
	}

	void SetOffset(const Vector3& Offset)
	{
		m_Offset = Offset;
	}

	void SetOffsetRotation(float x, float y, float z)
	{
		m_OffsetRot = Vector3(x, y, z);
	}

	void SetOffsetRotation(const Vector3& Offset)
	{
		m_OffsetRot = Offset;
	}

public:
	void Update(const Matrix& matBone);
	CSkeletonSocket* Clone();
};

