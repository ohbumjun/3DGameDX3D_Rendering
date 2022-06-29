
#include "DecalObj.h"


CDecalObj::CDecalObj()
{
	SetTypeID<CDecalObj>();
}

CDecalObj::CDecalObj(const CDecalObj& obj) :
	CGameObject(obj)
{
	m_Decal = (CDecalComponent*)FindComponent("Decal");
}

CDecalObj::~CDecalObj()
{
}

bool CDecalObj::Init()
{
	m_Decal = CreateComponent<CDecalComponent>("Decal");

	SetWorldPos(20.f, 0.f, 13.f);

	return true;
}

void CDecalObj::Update(float DeltaTime)
{
	CGameObject::Update(DeltaTime);
}

void CDecalObj::PostUpdate(float DeltaTime)
{
	CGameObject::PostUpdate(DeltaTime);
}

CDecalObj* CDecalObj::Clone()
{
	return new CDecalObj(*this);
}
