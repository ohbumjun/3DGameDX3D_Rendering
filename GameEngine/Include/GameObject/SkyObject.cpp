
#include "SkyObject.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneResource.h"
#include "../Resource/Material/Material.h"

CSkyObject::CSkyObject()
{
	SetTypeID<CSkyObject>();
}

CSkyObject::CSkyObject(const CSkyObject& obj) :
	CGameObject(obj)
{
	m_Mesh = (CStaticMeshComponent*)FindComponent("Mesh");
}

CSkyObject::~CSkyObject()
{
}

bool CSkyObject::Init()
{
	m_Mesh = CreateComponent<CStaticMeshComponent>("Mesh");

	m_Mesh->SetMesh("SpherePos");

	CMaterial* Material = m_Scene->GetResource()->FindMaterial("SkyMaterial");

	Material->SetRenderState("FrontFaceCull");
	Material->SetRenderState("SkyDepth");

	m_Mesh->AddMaterial(Material);

	m_Mesh->SetLayerName("Back");

	m_Mesh->SetRelativeScale(100000.f, 100000.f, 100000.f);

	return true;
}

void CSkyObject::Update(float DeltaTime)
{
	CGameObject::Update(DeltaTime);
}

void CSkyObject::PostUpdate(float DeltaTime)
{
	CGameObject::PostUpdate(DeltaTime);
}

CSkyObject* CSkyObject::Clone()
{
	return new CSkyObject(*this);
}
