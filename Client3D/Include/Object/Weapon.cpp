
#include "Weapon.h"

#include "Component/PickingLayerBox3D.h"

CWeapon::CWeapon()
{
	SetTypeID<CWeapon>();
}

CWeapon::CWeapon(const CWeapon& obj) :
	CGameObject(obj)
{
	m_Mesh = (CStaticMeshComponent*)FindComponent("Mesh");

	// Culling Area
	const Vector3& AnimComponentMeshSize = m_RootComponent->GetMeshSize();
	const Vector3& MeshRelativeScale = m_RootComponent->GetRelativeScale();

	// 아래 수치는 정석 (하지만 실제 크기는 이에 맞게 조정해줘야 한다)
	// Vector3 ColliderLength = Vector3(
	// 	AnimComponentMeshSize.x * MeshRelativeScale.x,
	// 	AnimComponentMeshSize.y * MeshRelativeScale.y,
	// 	AnimComponentMeshSize.z * MeshRelativeScale.z
	// );
	Vector3 ColliderLength = Vector3(
		AnimComponentMeshSize.x * MeshRelativeScale.x * 0.5f, // 위에서 써놨듯이, 조정을 적절히 해줘야 한다.
		AnimComponentMeshSize.y * MeshRelativeScale.y,
		AnimComponentMeshSize.z * MeshRelativeScale.z
	);

	m_CullingArea3D = CreateComponent<CPickingLayerBox3D>("ColliderBox3D");
	m_RootComponent->AddChild(m_CullingArea3D);
	m_CullingArea3D->SetLength(ColliderLength * 0.5f);
}

CWeapon::~CWeapon()
{
}

bool CWeapon::Init()
{
	m_Mesh = CreateComponent<CStaticMeshComponent>("Mesh");

	m_Mesh->SetMesh("Blade");
	m_Mesh->SetReceiveDecal(false);

	return true;
}

void CWeapon::Update(float DeltaTime)
{
	CGameObject::Update(DeltaTime);
}

void CWeapon::PostUpdate(float DeltaTime)
{
	CGameObject::PostUpdate(DeltaTime);
}

CWeapon* CWeapon::Clone()
{
	return new CWeapon(*this);
}
