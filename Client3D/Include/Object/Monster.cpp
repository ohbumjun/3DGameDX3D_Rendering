
#include "Monster.h"
#include "MonsterAnimation.h"

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



	m_Mesh->CreateAnimationInstance<CMonsterAnimation>();

	m_Animation = (CMonsterAnimation*)m_Mesh->GetAnimationInstance();

	m_Mesh->SetRelativeScale(0.02f, 0.02f, 0.02f);


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
