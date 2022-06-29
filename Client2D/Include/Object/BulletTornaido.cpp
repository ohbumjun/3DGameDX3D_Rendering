
#include "BulletTornaido.h"
#include "Component/SpriteComponent.h"

CBulletTornaido::CBulletTornaido()
{
	SetTypeID<CBulletTornaido>();
}

CBulletTornaido::CBulletTornaido(const CBulletTornaido& obj) :
	CGameObject(obj)
{
	m_Root = (CSceneComponent*)FindComponent("Root");
	m_Root1 = (CSceneComponent*)FindComponent("Root1");
	m_Sprite = (CSpriteComponent*)FindComponent("BulletSprite");
	m_Body = (CColliderCircle*)FindComponent("Body");
}

CBulletTornaido::~CBulletTornaido()
{
}

void CBulletTornaido::SetCollisionProfile(const std::string& Name)
{
	m_Body->SetCollisionProfile(Name);
}

bool CBulletTornaido::Init()
{
	m_Root = CreateComponent<CSceneComponent>("Root");
	m_Root1 = CreateComponent<CSceneComponent>("Root1");
	m_Sprite = CreateComponent<CSpriteComponent>("BulletSprite");
	m_Body = CreateComponent<CColliderCircle>("Body");

	SetRootComponent(m_Root);

	m_Root->AddChild(m_Root1);
	m_Root1->AddChild(m_Sprite);

	m_Sprite->AddChild(m_Body);

	m_Sprite->SetRelativeScale(50.f, 50.f, 1.f);
	m_Sprite->SetRelativePos(50.f, 0.f, 0.f);
	m_Sprite->SetPivot(0.5f, 0.5f, 0.f);
	m_Sprite->SetInheritRotZ(true);

	m_Body->AddCollisionCallback(Collision_State::Begin, this, &CBulletTornaido::OnCollisionBegin);
	m_Body->AddCollisionCallback(Collision_State::End, this, &CBulletTornaido::OnCollisionEnd);

	return true;
}

void CBulletTornaido::Update(float DeltaTime)
{
	CGameObject::Update(DeltaTime);

	AddRelativePos(GetWorldAxis(AXIS_Y) * 500.f * DeltaTime);

	m_Root1->AddRelativeRotationZ(720.f * DeltaTime);
}

void CBulletTornaido::PostUpdate(float DeltaTime)
{
	CGameObject::PostUpdate(DeltaTime);
}

CBulletTornaido* CBulletTornaido::Clone()
{
	return new CBulletTornaido(*this);
}

void CBulletTornaido::OnCollisionBegin(const CollisionResult& result)
{
	Destroy();
}

void CBulletTornaido::OnCollisionEnd(const CollisionResult& result)
{
}
