
#include "Portal.h"

CPortal::CPortal()
{
	SetTypeID<CPortal>();
}

CPortal::CPortal(const CPortal& obj) :
	CGameObject(obj)
{
	m_Billboard = (CBillboardComponent*)FindComponent("CBillboard");
}

CPortal::~CPortal()
{
}

bool CPortal::Init()
{
	m_Billboard = CreateComponent<CBillboardComponent>("CBillboard");

	SetWorldPos(15.f, 0.f, 13.f);
	SetWorldScale(5.f, 5.f, 1.f);

	return true;
}

CPortal* CPortal::Clone()
{
	return new CPortal(*this);
}
