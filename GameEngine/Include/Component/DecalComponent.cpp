
#include "DecalComponent.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneResource.h"
#include "../Resource/Mesh/StaticMesh.h"
#include "../Resource/Material/Material.h"
#include "../GameObject/GameObject.h"

CDecalComponent::CDecalComponent()	:
	m_FadeInTime(0.f),
	m_FadeInTimeAcc(0.f),
	m_FadeOutTime(0.f),
	m_FadeOutTimeAcc(0.f),
	m_DurationTime(0.f),
	m_DurationTimeAcc(0.f),
	m_FadeLoop(false),
	m_FadeState(Decal_Fade_State::Duration)
{
	SetTypeID<CDecalComponent>();
	m_Render = true;

	SetLayerName("Decal");
}

CDecalComponent::CDecalComponent(const CDecalComponent& com)	:
	CSceneComponent(com)
{
	m_Mesh = com.m_Mesh;

#ifdef _DEBUG
	m_DebugMesh = com.m_DebugMesh;
	m_DebugMaterial = com.m_DebugMaterial->Clone();
#endif // _DEBUG

	m_FadeLoop = com.m_FadeLoop;
	m_FadeInTime = com.m_FadeInTime;
	m_FadeInTimeAcc = com.m_FadeInTimeAcc;
	m_FadeOutTime = com.m_FadeOutTime;
	m_FadeOutTimeAcc = com.m_FadeOutTimeAcc;
	m_DurationTime = com.m_DurationTime;
	m_DurationTimeAcc = com.m_DurationTimeAcc;
	m_FadeState = com.m_FadeState;

	if (com.m_Material)
		m_Material = com.m_Material->Clone();

}

CDecalComponent::~CDecalComponent()
{
}

void CDecalComponent::SetMaterial(CMaterial* Material)
{
	m_Material = Material;
}

void CDecalComponent::Start()
{
	CSceneComponent::Start();

	if (m_FadeInTime > 0.f)
	{
		m_FadeState = Decal_Fade_State::FadeIn;

		if (m_Material)
			m_Material->SetOpacity(0.f);
	}

	else if (m_FadeOutTime > 0.f)
	{
		m_FadeState = Decal_Fade_State::FadeOut;

		if (m_Material)
			m_Material->SetOpacity(1.f);
	}

	else
		m_FadeState = Decal_Fade_State::Duration;
}

bool CDecalComponent::Init()
{
	if (!CSceneComponent::Init())
		return false;

#ifdef _DEBUG
	m_DebugMesh = (CStaticMesh*)m_Scene->GetResource()->FindMesh("CubeLinePos");
	m_DebugMaterial = m_Scene->GetResource()->FindMaterial("DebugDecal")->Clone();
#endif // _DEBUG

	m_Mesh = (CStaticMesh*)m_Scene->GetResource()->FindMesh("CubePos");
	m_Material = m_Scene->GetResource()->FindMaterial("DefaultDecal")->Clone();

	SetRelativeScale(3.f, 3.f, 3.f);

	return true;
}

void CDecalComponent::Update(float DeltaTime)
{
	CSceneComponent::Update(DeltaTime);

	switch (m_FadeState)
	{
	case Decal_Fade_State::FadeIn:
		m_FadeInTimeAcc += DeltaTime;

		m_Material->SetOpacity(m_FadeInTimeAcc / m_FadeInTime);

		if (m_FadeInTimeAcc >= m_FadeInTime)
		{
			m_FadeState = Decal_Fade_State::Duration;

			m_Material->SetOpacity(1.f);
		}
		break;
	case Decal_Fade_State::FadeOut:
		m_FadeOutTimeAcc += DeltaTime;

		m_Material->SetOpacity(1.f - m_FadeOutTimeAcc / m_FadeOutTime);

		if (m_FadeOutTimeAcc >= m_FadeOutTime)
		{
			if (m_FadeLoop)
			{
				m_FadeInTimeAcc = 0.f;
				m_FadeOutTimeAcc = 0.f;
				m_DurationTimeAcc = 0.f;

				if (m_FadeInTime > 0.f)
				{
					m_FadeState = Decal_Fade_State::FadeIn;
					m_Material->SetOpacity(0.f);
				}

				else
				{
					m_FadeState = Decal_Fade_State::Duration;
					m_Material->SetOpacity(1.f);
				}
			}

			else
				m_Object->Destroy();
		}
		break;
	case Decal_Fade_State::Duration:
		if (m_DurationTime > 0.f)
		{
			m_DurationTimeAcc += DeltaTime;

			if (m_DurationTimeAcc >= m_DurationTime)
			{
				if (m_FadeOutTime > 0.f)
					m_FadeState = Decal_Fade_State::FadeOut;

				else if (m_FadeLoop)
				{
					m_FadeInTimeAcc = 0.f;
					m_FadeOutTimeAcc = 0.f;
					m_DurationTimeAcc = 0.f;

					if (m_FadeInTime > 0.f)
					{
						m_FadeState = Decal_Fade_State::FadeIn;
						m_Material->SetOpacity(0.f);
					}
				}

				else
					m_Object->Destroy();
			}
		}
		break;
	}
}

void CDecalComponent::PostUpdate(float DeltaTime)
{
	CSceneComponent::PostUpdate(DeltaTime);
}

void CDecalComponent::PrevRender()
{
	CSceneComponent::PrevRender();
}

void CDecalComponent::Render()
{
	CSceneComponent::Render();

	m_Material->Render();

	m_Mesh->Render();

	m_Material->Reset();
}

void CDecalComponent::RenderDebug()
{
#ifdef _DEBUG
	CSceneComponent::RenderDebug();

	m_DebugMaterial->Render();

	m_DebugMesh->Render();

	m_DebugMaterial->Reset();

#endif // _DEBUG
}

void CDecalComponent::PostRender()
{
	CSceneComponent::PostRender();
}

CDecalComponent* CDecalComponent::Clone()
{
	return new CDecalComponent(*this);
}

void CDecalComponent::Save(FILE* File)
{
	CSceneComponent::Save(File);
}

void CDecalComponent::Load(FILE* File)
{
	CSceneComponent::Load(File);
}
