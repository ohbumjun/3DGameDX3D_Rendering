
#include "Bullet.h"
#include "Component/SpriteComponent.h"

CBullet::CBullet()	:
	m_Distance(600.f)
{
	SetTypeID<CBullet>();
}

CBullet::CBullet(const CBullet& obj) :
	CGameObject(obj)
{
	m_Sprite = (CSpriteComponent*)FindComponent("BulletSprite");
	m_Body = (CColliderCircle*)FindComponent("Body");
	m_Distance = obj.m_Distance;
}

CBullet::~CBullet()
{
}

void CBullet::SetCollisionProfile(const std::string& Name)
{
	m_Body->SetCollisionProfile(Name);
}

bool CBullet::Init()
{
	m_Sprite = CreateComponent<CSpriteComponent>("BulletSprite");
	m_Body = CreateComponent<CColliderCircle>("Body");

	SetRootComponent(m_Sprite);

	m_Sprite->AddChild(m_Body);

	m_Sprite->SetRelativeScale(50.f, 50.f, 1.f);
	m_Sprite->SetPivot(0.5f, 0.5f, 0.f);

	m_Body->AddCollisionCallback(Collision_State::Begin, this, &CBullet::OnCollisionBegin);
	m_Body->AddCollisionCallback(Collision_State::End, this, &CBullet::OnCollisionEnd);

	return true;
}

void CBullet::Update(float DeltaTime)
{
	CGameObject::Update(DeltaTime);

	float	Dist = 500.f * DeltaTime;

	m_Distance -= Dist;

	if (m_Distance <= 0.f)
	{
		Destroy();
	}

	AddRelativePos(GetWorldAxis(AXIS_Y) * Dist);
}

void CBullet::PostUpdate(float DeltaTime)
{
	CGameObject::PostUpdate(DeltaTime);
}

CBullet* CBullet::Clone()
{
	return new CBullet(*this);
}

void CBullet::OnCollisionBegin(const CollisionResult& result)
{
	Destroy();
}

void CBullet::OnCollisionEnd(const CollisionResult& result)
{
}
