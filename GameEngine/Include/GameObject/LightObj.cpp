
#include "LightObj.h"

CLightObj::CLightObj()
{
	SetTypeID<CLightObj>();
}

CLightObj::CLightObj(const CLightObj& obj)	:
	CGameObject(obj)
{
	m_Light = (CLightComponent*)FindComponent("Light");
}

CLightObj::~CLightObj()
{
}

bool CLightObj::Init()
{
	m_Light = CreateComponent<CLightComponent>("Light");

	return true;
}

void CLightObj::Update(float DeltaTime)
{
	CGameObject::Update(DeltaTime);
}

void CLightObj::PostUpdate(float DeltaTime)
{
	CGameObject::PostUpdate(DeltaTime);
}

CLightObj* CLightObj::Clone()
{
	return new CLightObj(*this);
}
