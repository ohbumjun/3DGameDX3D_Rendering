#pragma once
#include "SceneComponent.h"
class CArm :
    public CSceneComponent
{
	friend class CGameObject;
	friend class CCameraManager;

protected:
	CArm();
	CArm(const CArm& com);
	virtual ~CArm();

protected:
	float	m_TargetDistance;
	Vector3	m_Offset;

public:
	float GetTargetDistance()	const
	{
		return m_TargetDistance;
	}

	const Vector3& GetOffset()	const
	{
		return m_Offset;
	}

public:
	void SetOffset(const Vector3& Offset)
	{
		m_Offset = Offset;
	}

	void SetOffset(float x, float y, float z)
	{
		Vector3 Offset(x, y, z);

		m_Offset = Offset;
	}

	void SetTargetDistance(float Distance)
	{
		m_TargetDistance = Distance;
	}

public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);
	virtual void PrevRender();
	virtual void Render();
	virtual void PostRender();
	virtual CArm* Clone();
	virtual void Save(FILE* File);
	virtual void Load(FILE* File);
};

