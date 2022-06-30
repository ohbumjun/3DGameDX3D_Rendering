
#include "Player.h"
#include "PlayerAnimation.h"
#include "Input.h"
#include "Scene/Scene.h"
#include "Scene/Navigation3DManager.h"
#include "Weapon.h"
#include "Component/ColliderBox3D.h"

CPlayer::CPlayer()
{
	SetTypeID<CPlayer>();
}

CPlayer::CPlayer(const CPlayer& obj)	:
	CGameObject(obj)
{
	m_Mesh = (CAnimationMeshComponent*)FindComponent("Mesh");
	m_Arm = (CArm*)FindComponent("Arm");
	m_Camera = (CCameraComponent*)FindComponent("Camera");
	m_NavAgent = (CNavAgent*)FindComponent("NavAgent");
}

CPlayer::~CPlayer()
{
}

bool CPlayer::Init()
{
	m_Mesh = CreateComponent<CAnimationMeshComponent>("PlayerMesh");
	m_Arm = CreateComponent<CArm>("Arm");
	m_Camera = CreateComponent<CCameraComponent>("Camera");
	m_NavAgent = CreateComponent<CNavAgent>("NavAgent");
	m_ColliderBox3D = CreateComponent<CColliderBox3D>("ColliderBox3D");

	// Arm, Camera
	m_Mesh->AddChild(m_Arm);
	m_Arm->AddChild(m_Camera);

	m_Camera->SetInheritRotX(true);
	m_Camera->SetInheritRotY(true);
	m_Camera->SetInheritRotZ(true);

	// Set Mesh 를 해줘야만 MeshSize가 세팅 (SetMeshSize)
	// - Root Component 인 Animation Mesh Component 의 Transform 에 MeshSize 에 대한 정보가 들어있게 된다. (min, max)
	m_Mesh->SetMesh("PlayerMesh");
	m_Mesh->SetReceiveDecal(false);

	// Animation
	m_Mesh->CreateAnimationInstance<CPlayerAnimation>();
	m_Animation = (CPlayerAnimation*)m_Mesh->GetAnimationInstance();

	// Scale
	m_Mesh->SetRelativeScale(0.02f, 0.02f, 0.02f);
	// m_Mesh->SetWorldPos(1.f, 0.f, 1.f);
	// 이걸 해도 되는 것인가 ?

	m_Arm->SetOffset(0.f, 2.f, 0.f);
	m_Arm->SetRelativeRotation(25.f, 0.f, 0.f);
	m_Arm->SetTargetDistance(10.f);

	// Collider
	m_Mesh->AddChild(m_ColliderBox3D);

	const Vector3& AnimComponentMeshSize = m_Mesh->GetMeshSize();
	const Vector3& MeshWorldScale = m_Mesh->GetWorldScale();
	Vector3 ColliderLength = Vector3(
		AnimComponentMeshSize.x * MeshWorldScale.x,
		AnimComponentMeshSize.y * MeshWorldScale.y,
		AnimComponentMeshSize.z * MeshWorldScale.z
	);
	
	// Center 지점의 경우, 기본적으로 Player 의 WorldPos 가 발밑으로 잡힌다.
	// 즉, 아무 처리를 해주지 않을 경우, Center 가 발밑으로 잡힌다는 의미이다.
	
	// m_ColliderBox3D->SetInfo(
	// 	m_RootComponent->GetWorldPos() - AnimComponentMeshSize * m_RootComponent->GetPivot(),
	// 	ColliderLength * 0.5f);

	// m_Body->SetCollisionProfile("Player");
	// SetInfo(const Vector3 & Center, const Vector3 & Length)

	// Weapon 달지 말고
	// m_Weapon = m_Scene->CreateGameObject<CWeapon>("Weapon");
	// m_Mesh->AddChild(m_Weapon, "Weapon");

	CInput::GetInst()->SetKeyCallback<CPlayer>("MoveFront", KeyState_Push,
		this, &CPlayer::MoveFront);
	CInput::GetInst()->SetKeyCallback<CPlayer>("MoveBack", KeyState_Push,
		this, &CPlayer::MoveBack);
	CInput::GetInst()->SetKeyCallback<CPlayer>("RotationYInv", KeyState_Push,
		this, &CPlayer::RotationYInv);
	CInput::GetInst()->SetKeyCallback<CPlayer>("RotationY", KeyState_Push,
		this, &CPlayer::RotationY);

	CInput::GetInst()->SetKeyCallback<CPlayer>("MovePoint", KeyState_Down,
		this, &CPlayer::MovePoint);

	CInput::GetInst()->SetKeyCallback<CPlayer>("Attack1", KeyState_Down,
		this, &CPlayer::Attack);


	return true;
}

void CPlayer::Update(float DeltaTime)
{
	CGameObject::Update(DeltaTime);

	if (CInput::GetInst()->GetWheelDir())
	{
		float Length = m_Arm->GetTargetDistance() +
			CInput::GetInst()->GetWheelDir() * 0.1f;

		m_Arm->SetTargetDistance(Length);
	}

	//m_Arm->AddRelativeRotationY(90.f * DeltaTime);
	if (m_Velocity.Length() > 0.f)
	{
		//m_Animation->ChangeAnimation("Walk");
		m_Animation->SetIdleEnable(true);
	}

	else if(m_Animation->GetIdleEnable())
	{
		m_Animation->ChangeAnimation("Idle");
	}

	CGameObject* PickObj = nullptr;

	if (m_Scene->Picking(PickObj))
	{
		int a = 0;
	}
}

void CPlayer::PostUpdate(float DeltaTime)
{
	CGameObject::PostUpdate(DeltaTime);

	//Vector3	Pos = GetWorldPos();

	//Pos.y = m_Scene->GetNavigation3DManager()->GetY(Pos);

	//SetWorldPos(Pos);

	m_Velocity = Vector3::Zero;
}

CPlayer* CPlayer::Clone()
{
	return new CPlayer(*this);
}

void CPlayer::MoveFront(float DeltaTime)
{
	m_Velocity += GetWorldAxis(AXIS_Z) * 10.f * DeltaTime;

	AddWorldPos(GetWorldAxis(AXIS_Z) * 10.f * DeltaTime);
}

void CPlayer::MoveBack(float DeltaTime)
{
	m_Velocity += GetWorldAxis(AXIS_Z) * -10.f * DeltaTime;

	AddWorldPos(GetWorldAxis(AXIS_Z) * -10.f * DeltaTime);
}

void CPlayer::RotationYInv(float DeltaTime)
{
	AddWorldRotationY(-180.f * DeltaTime);
}

void CPlayer::RotationY(float DeltaTime)
{
	AddWorldRotationY(180.f * DeltaTime);
}

void CPlayer::Attack(float DeltaTime)
{
	m_Animation->ChangeAnimation("Attack");
	m_Animation->SetIdleEnable(false);
}

void CPlayer::MovePoint(float DeltaTime)
{
	Vector3 Point = m_Scene->GetNavigation3DManager()->GetPickingPos();

	Move(Point);
}
