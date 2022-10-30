
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

	// 즉, 원형 구에 SkyBox를 입히고, 우리는 그 안에서 밖을 바라보는 형태
	m_Mesh->SetMesh("SpherePos");

	CMaterial* Material = m_Scene->GetResource()->FindMaterial("SkyMaterial");

	// 안쪽에서 밖을 향해 볼 수 있어야 한다.
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
