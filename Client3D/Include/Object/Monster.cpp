
#include "Monster.h"
#include "MonsterAnimation.h"
#include "Component/ColliderSphere.h"
#include "Component/ColliderBox3D.h"

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
	/*
	m_ColliderBox3D = CreateComponent<CColliderBox3D>("ColliderBox3D");
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

	Vector3 ColliderCenter = Vector3(
		GetWorldPos().x,
		GetWorldPos().y + AnimComponentMeshSize.y * MeshRelativeScale.y * 0.5f,
		GetWorldPos().z
	);

	// Center 지점의 경우, 기본적으로 Player 의 WorldPos 가 발밑으로 잡힌다.
	// 즉, 아무 처리를 해주지 않을 경우, Center 가 발밑으로 잡힌다는 의미이다.
	// MeshSize y만큼 0.5 올려서 Center 를 잡을 것이다.
	// 해당 변수 내용을 이용해도 된다.
	m_ColliderBox3D->SetInfo(ColliderCenter, ColliderLength * 0.5f);
	*/


	// Collider Sphere
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
