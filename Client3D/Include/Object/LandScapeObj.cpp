
#include "LandScapeObj.h"

CLandScapeObj::CLandScapeObj()
{
	SetTypeID<CLandScapeObj>();
}

CLandScapeObj::CLandScapeObj(const CLandScapeObj& obj) :
	CGameObject(obj)
{
	m_LandScape	 = (CLandScape*)FindComponent("LandScape");
}

CLandScapeObj::~CLandScapeObj()
{
}

bool CLandScapeObj::Init()
{
	m_LandScape = CreateComponent<CLandScape>("LandScape");

	m_LandScape->CreateLandScape("LandScape", 129, 129,
		TEXT("LandScape/height1.bmp"));

	m_LandScape->AddMaterial("LandScape");

	m_LandScape->SetDetailLevel(30.f);
	m_LandScape->SetSplatCount(4);

	return true;
}

void CLandScapeObj::Update(float DeltaTime)
{
	CGameObject::Update(DeltaTime);
}

void CLandScapeObj::PostUpdate(float DeltaTime)
{
	CGameObject::PostUpdate(DeltaTime);
}

CLandScapeObj* CLandScapeObj::Clone()
{
	return new CLandScapeObj(*this);
}
