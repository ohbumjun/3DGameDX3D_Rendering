
#include "BonObj.h"

#include "Component/PickingLayerBox3D.h"

CBonObj::CBonObj()
{
	SetTypeID<CBonObj>();
}

CBonObj::CBonObj(const CBonObj& obj) :
	CGameObject(obj)
{
	m_Mesh = (CStaticMeshComponent*)FindComponent("Mesh");
}

CBonObj::~CBonObj()
{
}

bool CBonObj::Init()
{
	m_Mesh = CreateComponent<CStaticMeshComponent>("Mesh");

	m_Mesh->SetMesh("BonObj");

	SetRelativeScale(0.03f, 0.03f, 0.03f);


	const Vector3& AnimComponentMeshSize = m_Mesh->GetMeshSize();
	const Vector3& MeshRelativeScale = m_Mesh->GetRelativeScale();

	// 아래 수치는 정석 (하지만 실제 크기는 이에 맞게 조정해줘야 한다)
	// Vector3 ColliderLength = Vector3(
	// 	AnimComponentMeshSize.x * MeshRelativeScale.x,
	// 	AnimComponentMeshSize.y * MeshRelativeScale.y,
	// 	AnimComponentMeshSize.z * MeshRelativeScale.z
	// );
	Vector3 ColliderLength = Vector3(
		AnimComponentMeshSize.x * MeshRelativeScale.x, 
		AnimComponentMeshSize.y * MeshRelativeScale.y,
		AnimComponentMeshSize.z * MeshRelativeScale.z
	);

	m_CullingArea3D = CreateComponent<CPickingLayerBox3D>("ColliderBox3D");
	m_Mesh->AddChild(m_CullingArea3D);
	m_CullingArea3D->SetLength(ColliderLength * 0.5f);

	return true;
}

void CBonObj::Update(float DeltaTime)
{
	CGameObject::Update(DeltaTime);
}

void CBonObj::PostUpdate(float DeltaTime)
{
	CGameObject::PostUpdate(DeltaTime);
}

CBonObj* CBonObj::Clone()
{
	return new CBonObj(*this);
}
