#pragma once

#include "SceneComponent.h"
#include "../Resource/Mesh/StaticMesh.h"
#include "../Resource/Material/Material.h"

enum class Decal_Fade_State
{
    FadeIn,
    FadeOut,
    Duration
};

class CDecalComponent :
    public CSceneComponent
{
	friend class CGameObject;
	friend class CCameraManager;

protected:
	CDecalComponent();
	CDecalComponent(const CDecalComponent& com);
	virtual ~CDecalComponent();

protected:
#ifdef _DEBUG
	CSharedPtr<CStaticMesh> m_DebugMesh;
	CSharedPtr<CMaterial> m_DebugMaterial;
#endif // _DEBUG

	CSharedPtr<CStaticMesh> m_Mesh;
	CSharedPtr<CMaterial> m_Material;
	Decal_Fade_State	m_FadeState;

	float	m_FadeInTime;
	float	m_FadeInTimeAcc;
	float	m_FadeOutTime;
	float	m_FadeOutTimeAcc;
	float	m_DurationTime;
	float	m_DurationTimeAcc;

	bool	m_FadeLoop;

public:
	void SetFadeInTime(float Time)
	{
		m_FadeInTime = Time;
	}

	void SetFadeOutTime(float Time)
	{
		m_FadeOutTime = Time;
	}

	void SetDuration(float Time)
	{
		m_DurationTime = Time;
	}

	void SetFadeLoop(bool Loop)
	{
		m_FadeLoop = Loop;
	}

	void SetMaterial(CMaterial* Material);

public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);
	virtual void PrevRender();
	virtual void Render();
	virtual void RenderDebug();
	virtual void PostRender();
	virtual CDecalComponent* Clone();
	virtual void Save(FILE* File);
	virtual void Load(FILE* File);
};

