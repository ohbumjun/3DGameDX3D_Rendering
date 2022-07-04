
#include "Monster.h"
#include "MonsterAnimation.h"
#include "Component/ColliderSphere.h"
#include "Component/ColliderBox3D.h"
#include "Component/PickingLayerBox3D.h"

CMonster::CMonster()
{
	SetTypeID<CMonster>();
}

CMonster::CMonster(const CMonster& obj) :
	CGameObject(obj)
{
	m_Mesh = (CAnimationMeshComponent*)FindComponent("Mesh");
}

CMonster::~CMonster()
{
}

bool CMonster::Init()
{
	m_Mesh = CreateComponent<CAnimationMeshComponent>("MonsterMesh");

	m_Mesh->SetMesh("PlayerMesh");
	m_Mesh->SetReceiveDecal(false);
	m_Mesh->SetRelativeScale(0.02f, 0.02f, 0.02f);

	// Animation
	m_Mesh->CreateAnimationInstance<CMonsterAnimation>();
	m_Animation = (CMonsterAnimation*)m_Mesh->GetAnimationInstance();

	// Collider Box
	m_ColliderBox3D = CreateComponent<CColliderBox3D>("ColliderBox3D");
	m_ColliderBox3D->SetCollisionProfile("Monster");
	m_Mesh->AddChild(m_ColliderBox3D);

	const Vector3& AnimComponentMeshSize = m_Mesh->GetMeshSize();
	const Vector3& MeshRelativeScale = m_Mesh->GetRelativeScale();

	// 아래 수치는 정석 (하지만 실제 크기는 이에 맞게 조정해줘야 한다)
	// Vector3 ColliderLength = Vector3(
	// 	AnimComponentMeshSize.x * MeshRelativeScale.x,
	// 	AnimComponentMeshSize.y * MeshRelativeScale.y,
	// 	AnimComponentMeshSize.z * MeshRelativeScale.z
	// );
	Vector3 ColliderLength = Vector3(
		AnimComponentMeshSize.x * MeshRelativeScale.x * 0.5f,
		AnimComponentMeshSize.y * MeshRelativeScale.y,
		AnimComponentMeshSize.z * MeshRelativeScale.z
	);

	// Root Component 의 World 공간, SphereInfo 의 Center 값을 세팅
	const Vector3& ColliderCenter = m_RootComponent->GetSphereInfo().Center;

	m_ColliderBox3D->SetInfo(ColliderCenter, ColliderLength * 0.5f);


	// Collider Sphere
	/*
	m_ColliderSphere = CreateComponent<CColliderSphere>("ColliderSphere");
	m_Mesh->AddChild(m_ColliderSphere);
	m_ColliderSphere->SetCollisionProfile("Monster");
	const Vector3& AnimComponentMeshSize = m_Mesh->GetMeshSize();
	const Vector3& MeshRelativeScale = m_Mesh->GetRelativeScale();
	Vector3 ColliderCenter = Vector3(
		GetWorldPos().x,
		GetWorldPos().y + AnimComponentMeshSize.y * MeshRelativeScale.y * 0.5f,
		GetWorldPos().z
	);

	float ColliderRadiius = AnimComponentMeshSize.x * MeshRelativeScale.x;
	ColliderRadiius = AnimComponentMeshSize.y * MeshRelativeScale.y < ColliderRadiius ?
		AnimComponentMeshSize.y * MeshRelativeScale.y :ColliderRadiius;
	ColliderRadiius = AnimComponentMeshSize.z * MeshRelativeScale.z < ColliderRadiius ?
		AnimComponentMeshSize.z * MeshRelativeScale.z :ColliderRadiius;

	m_ColliderSphere->SetInfo(ColliderCenter, ColliderRadiius);
	*/

	m_CullingArea3D = CreateComponent<CPickingLayerBox3D>("ColliderBox3D");
	m_Mesh->AddChild(m_CullingArea3D);
	m_CullingArea3D->SetLength(ColliderLength * 0.5f);

	return true;
}

void CMonster::Update(float DeltaTime)
{
	CGameObject::Update(DeltaTime);
}

void CMonster::PostUpdate(float DeltaTime)
{
	CGameObject::PostUpdate(DeltaTime);
}

CMonster* CMonster::Clone()
{
	return new CMonster(*this);
}
