
#include "BonObj.h"


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
