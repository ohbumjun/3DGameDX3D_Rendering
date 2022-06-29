
#include "PixelTest.h"
#include "Component/ColliderPixel.h"

CPixelTest::CPixelTest()
{
	SetTypeID<CPixelTest>();
}

CPixelTest::CPixelTest(const CPixelTest& obj) :
	CGameObject(obj)
{
	m_Body = (CColliderPixel*)FindComponent("Body");
}

CPixelTest::~CPixelTest()
{
}

bool CPixelTest::Init()
{
	m_Body = CreateComponent<CColliderPixel>("Body");

	SetRootComponent(m_Body);

	m_Body->SetRelativePos(400.f, 50.f, 0.f);

	m_Body->SetInfo(TEXT("PixelCollision.png"));
	m_Body->SetPixelCollisionType(PixelCollision_Type::Color_Ignore);
	m_Body->SetPixelColor(255, 0, 255);

	return true;
}

CPixelTest* CPixelTest::Clone()
{
	return new CPixelTest(*this);
}
