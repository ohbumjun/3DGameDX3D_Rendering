
#include "LightComponent.h"
#include "../Scene/Scene.h"
#include "../Scene/LightManager.h"
#include "../Scene/CameraManager.h"
#include "CameraComponent.h"

CLightComponent::CLightComponent()
{
	SetTypeID<CLightComponent>();
	m_Render = false;

	m_CBuffer = nullptr;
}

CLightComponent::CLightComponent(const CLightComponent& com) :
	CSceneComponent(com)
{
	m_CBuffer = com.m_CBuffer->Clone();
}

CLightComponent::~CLightComponent()
{
	m_Scene->GetLightManager()->DeleteLight(this);
	SAFE_DELETE(m_CBuffer);
}

void CLightComponent::Start()
{
	CSceneComponent::Start();

	m_Scene->GetLightManager()->AddLight(this);
}

bool CLightComponent::Init()
{
	m_CBuffer = new CLightConstantBuffer;
	
	m_CBuffer->Init();

	return true;
}

void CLightComponent::Update(float DeltaTime)
{
	CSceneComponent::Update(DeltaTime);
}

void CLightComponent::PostUpdate(float DeltaTime)
{
	CSceneComponent::PostUpdate(DeltaTime);

	CCameraComponent* Camera = m_Scene->GetCameraManager()->GetCurrentCamera();

	if (m_CBuffer->GetLightType() != Light_Type::Dir)
	{
		Vector3	Pos = GetWorldPos();

		// 뷰 공간으로 변환한다.
		Pos = Pos.TransformCoord(Camera->GetViewMatrix());

		m_CBuffer->SetPos(Pos);

		if (m_CBuffer->GetLightType() == Light_Type::Point)
			SetRelativeScale(m_CBuffer->GetLightDistance(), m_CBuffer->GetLightDistance(), m_CBuffer->GetLightDistance());
	}

	if(m_CBuffer->GetLightType() != Light_Type::Point)
	{
		Vector3 Dir = GetWorldAxis(AXIS_Z);

		// 뷰공간으로 변환한다.
		Dir = Dir.TransformNormal(Camera->GetViewMatrix());
		Dir.Normalize();

		m_CBuffer->SetDir(Dir);
	}
}

void CLightComponent::PrevRender()
{
	CSceneComponent::PrevRender();
}

void CLightComponent::Render()
{
	CSceneComponent::Render();
}

void CLightComponent::PostRender()
{
	CSceneComponent::PostRender();
}

CLightComponent* CLightComponent::Clone()
{
	return new CLightComponent(*this);
}

void CLightComponent::Save(FILE* File)
{
	CSceneComponent::Save(File);
}

void CLightComponent::Load(FILE* File)
{
	CSceneComponent::Load(File);
}

void CLightComponent::SetShader()
{
	m_CBuffer->UpdateCBuffer();
}
