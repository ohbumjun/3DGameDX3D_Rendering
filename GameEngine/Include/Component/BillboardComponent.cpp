
#include "BillboardComponent.h"
#include "../GameObject/GameObject.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneResource.h"
#include "../Scene/CameraManager.h"
#include "CameraComponent.h"

CBillboardComponent::CBillboardComponent()
{
	SetTypeID<CBillboardComponent>();
	m_Render = true;

	SetLayerName("Particle");
}

CBillboardComponent::CBillboardComponent(const CBillboardComponent& com) :
	CSceneComponent(com)
{
	m_Mesh = com.m_Mesh;

	if (com.m_Material)
		m_Material = com.m_Material->Clone();
}

CBillboardComponent::~CBillboardComponent()
{
}

void CBillboardComponent::SetMaterial(const std::string& Name)
{
	m_Material = m_Scene->GetResource()->FindMaterial(Name);
}

void CBillboardComponent::SetMaterial(CMaterial* Material)
{
	m_Material = Material;
}

void CBillboardComponent::Start()
{
	CSceneComponent::Start();
}

bool CBillboardComponent::Init()
{
	if (!CSceneComponent::Init())
		return false;

	m_Mesh = (CSpriteMesh*)m_Scene->GetResource()->FindMesh("SpriteMesh");
	m_Material = m_Scene->GetResource()->FindMaterial("Billboard")->Clone();

	SetPivot(0.5f, 0.5f, 0.f);
	SetRelativeScale(1.f, 1.f, 1.f);
	SetMeshSize(1.f, 1.f, 0.f);

	return true;
}

void CBillboardComponent::Update(float DeltaTime)
{
	CSceneComponent::Update(DeltaTime);

	// 카메라의 위치를 얻어온다.
	Vector3 CameraPos = m_Scene->GetCameraManager()->GetCurrentCamera()->GetWorldPos();

	Vector3	View = CameraPos - GetWorldPos();
	View.Normalize();

	Vector3 OriginDir(0.f, 0.f, -1.f);

	m_Transform->SetRotationAxis(OriginDir, View);
}

void CBillboardComponent::PostUpdate(float DeltaTime)
{
	CSceneComponent::PostUpdate(DeltaTime);
}

void CBillboardComponent::PrevRender()
{
	CSceneComponent::PrevRender();
}

void CBillboardComponent::Render()
{
	CSceneComponent::Render();

	m_Material->Render();

	m_Mesh->Render();

	m_Material->Reset();
}

void CBillboardComponent::RenderDebug()
{
	CSceneComponent::RenderDebug();
}

void CBillboardComponent::PostRender()
{
	CSceneComponent::PostRender();
}

CBillboardComponent* CBillboardComponent::Clone()
{
	return new CBillboardComponent(*this);
}

void CBillboardComponent::Save(FILE* File)
{
	CSceneComponent::Save(File);
}

void CBillboardComponent::Load(FILE* File)
{
	CSceneComponent::Load(File);
}
