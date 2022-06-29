
#include "Weapon.h"

CWeapon::CWeapon()
{
	SetTypeID<CWeapon>();
}

CWeapon::CWeapon(const CWeapon& obj) :
	CGameObject(obj)
{
	m_Mesh = (CStaticMeshComponent*)FindComponent("Mesh");
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
