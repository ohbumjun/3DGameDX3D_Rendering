
#include "BulletCamera.h"
#include "Scene/Scene.h"
#include "Scene/CameraManager.h"

CBulletCamera::CBulletCamera() :
	m_Distance(600.f)
{
	SetTypeID<CBulletCamera>();
}

CBulletCamera::CBulletCamera(const CBulletCamera& obj) :
	CGameObject(obj)
{
	m_Sprite = (CSpriteComponent*)FindComponent("BulletSprite");
	m_Body = (CColliderBox2D*)FindComponent("BulletBody");
	m_Camera = (CCameraComponent*)FindComponent("BulletCamera");
	m_Distance = obj.m_Distance;
}

CBulletCamera::~CBulletCamera()
{
}

void CBulletCamera::SetCollisionProfile(const std::string& Name)
{
	m_Body->SetCollisionProfile(Name);
}

void CBulletCamera::Start()
{
	CGameObject::Start();

	m_Scene->GetCameraManager()->KeepCamera();
	m_Scene->GetCameraManager()->SetCurrentCamera(m_Camera);
}

bool CBulletCamera::Init()
{
	CGameObject::Init();

	m_Sprite = CreateComponent<CSpriteComponent>("BulletSprite");
	m_Body = CreateComponent<CColliderBox2D>("BulletBody");
	m_Camera = CreateComponent<CCameraComponent>("BulletCamera");

	SetRootComponent(m_Sprite);

	m_Sprite->AddChild(m_Body);
	m_Sprite->AddChild(m_Camera);

	//m_Body->SetInheritRotZ(true);

	m_Camera->OnViewportCenter();

	m_Body->SetCollisionProfile("PlayerAttack");
	m_Body->AddCollisionCallback<CBulletCamera>(Collision_State::Begin, this,
		&CBulletCamera::CollisionCallback);

	m_Sprite->SetRelativeScale(50.f, 50.f, 1.f);
	m_Sprite->SetPivot(0.5f, 0.5f, 0.f);

	return true;
}

void CBulletCamera::Update(float DeltaTime)
{
	CGameObject::Update(DeltaTime);

	float	Dist = 500.f * DeltaTime;

	m_Distance -= Dist;

	if (m_Distance <= 0.f)
	{
		m_Scene->GetCameraManager()->ReturnCamera();
		Destroy();
	}

	AddRelativePos(GetWorldAxis(AXIS_Y) * Dist);
}

void CBulletCamera::PostUpdate(float DeltaTime)
{
	CGameObject::PostUpdate(DeltaTime);
}

CBulletCamera* CBulletCamera::Clone()
{
	return new CBulletCamera(*this);
}

void CBulletCamera::CollisionCallback(const CollisionResult& result)
{
	m_Scene->GetCameraManager()->ReturnCamera();
	Destroy();
}
