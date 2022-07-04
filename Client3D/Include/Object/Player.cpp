
#include "Player.h"
#include "PlayerAnimation.h"
#include "Input.h"
#include "Scene/Scene.h"
#include "Scene/Navigation3DManager.h"
#include "Weapon.h"
#include "Component/ColliderBox3D.h"
#include "Component/ColliderSphere.h"
#include "Component/PickingLayerBox3D.h"

// 1. 왜 Player 의 SphereInfo.Center 는 0이 아니라 음수가 세팅될까 ? (y좌표)
// -> 아래에서 m_Mesh->SetMesh(PlayerMesh) 를 하면서, Load한 fbx Mesh 정보를 세팅한다.
// -> 이때, 해당 Mesh 에서 min, max 정보가 있는데, min이 0에서 시작하는 것이 아니다. 각자 그때그때 다른 min, max 가 세팅되어 있다.
// -> 이에 따라 SphereInfo.Center 도 다르게 세팅되는 것이다.

// 2. 그러면 왜 Player 의 발밑이 min 이 아니라, Center 에서 시작하는 것일까 ?
// 즉, 왜 SphereInfo.Center 가 Player 의 발밑으로 세팅되는 것일까 ?
// -> 말이 틀렸다. Player 의 발밑은, Player 의 World Pos 이다. 그리고 이 값은 처음에 (0.f, 0.f, 0.f) 이다
// -> 반면, SphereInfo.Center 의 y값은 미묘하게 -0.34 이다.
// -> 즉, Mesh 의 Min 자체가 0에서 시작하는 것이 아니기 때문에 SphereInfo.Center 와, 실제 물체의 중간 위치가 일치하지 않는 것
// -> 반면, LandScape 의 경우, Min이 0에서 시작하게 코드가 되어 있다. 그래서 Center 와 실제 중간 위치가 일치하는 것

// 3. 어떻게 하면 일관되게 Center 정보를 세팅할 수 있을까 ?
// -> 모든 min 을 0 에서 시작하도록 조정해주면 되는 것 아닐까 ?
// -> 아니다. 원본 Mesh 정보는 그대로 세팅하는 것이 좋을 것 같다.
// -> 그저 SphereInfo.Center 정보만 WorldPos 와 Mesh 크기에 맞게 조정해주면 될 것 같다.
// -> Animation Mesh 혹은 Static Mesh Component 에 대해서만 적용하면 될 것 같다.
// -> 

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

	// Arm, Camera
	m_Mesh->AddChild(m_Arm);
	m_Arm->AddChild(m_Camera);

	m_Camera->SetInheritRotX(true);
	m_Camera->SetInheritRotY(true);
	m_Camera->SetInheritRotZ(true);

	// Set Mesh 를 해줘야만 MeshSize가 세팅 (SetMeshSize)
	// - Root Component 인 Animation Mesh Component 의 Transform 에 MeshSize 에 대한 정보가 들어있게 된다. 
	// 여기에서 Min, Max 정보가 세팅되고, SphereInfo Center, Radius 정보가 세팅된다.
 	m_Mesh->SetMesh("PlayerMesh");
	m_Mesh->SetReceiveDecal(false);

	// Animation
	m_Mesh->CreateAnimationInstance<CPlayerAnimation>();
	m_Animation = (CPlayerAnimation*)m_Mesh->GetAnimationInstance();

	// Scale
	m_Mesh->SetRelativeScale(0.02f, 0.02f, 0.02f);

	m_Arm->SetOffset(0.f, 2.f, 0.f);
	m_Arm->SetRelativeRotation(25.f, 0.f, 0.f);
	m_Arm->SetTargetDistance(10.f);

	const Vector3& AnimComponentMeshSize = m_Mesh->GetMeshSize();
	const Vector3& MeshRelativeScale = m_Mesh->GetRelativeScale();

	// 아래 수치는 정석 (하지만 실제 크기는 이에 맞게 조정해줘야 한다)
	// Vector3 ColliderLength = Vector3(
	// 	AnimComponentMeshSize.x * MeshRelativeScale.x,
	// 	AnimComponentMeshSize.y * MeshRelativeScale.y,
	// 	AnimComponentMeshSize.z * MeshRelativeScale.z
	// );

	Vector3 ColliderLength = Vector3(
		AnimComponentMeshSize.x * MeshRelativeScale.x * 0.5f, // 위에서 써놨듯이, 조정을 적절히 해줘야 한다.
		AnimComponentMeshSize.y * MeshRelativeScale.y,
		AnimComponentMeshSize.z * MeshRelativeScale.z
	);

	// Collider
	m_ColliderBox3D = CreateComponent<CColliderBox3D>("ColliderBox3D");
	m_ColliderBox3D->SetCollisionProfile("Player");
	m_Mesh->AddChild(m_ColliderBox3D);

	// Center 지점의 경우, 기본적으로 Player 의 WorldPos 가 발밑으로 잡힌다.
	// 즉, 아무 처리를 해주지 않을 경우, Center 가 발밑으로 잡힌다는 의미이다.
	// MeshSize y만큼 0.5 올려서 Center 를 잡을 것이다.
	// 해당 변수 내용을 이용해도 된다.
	m_ColliderBox3D->SetInfo(m_RootComponent->GetSphereOriginInfo().Center, ColliderLength * 0.5f);

	/*
	m_ColliderSphere = CreateComponent<CColliderSphere>("ColliderSphere");
	m_Mesh->AddChild(m_ColliderSphere);
	m_ColliderSphere->SetCollisionProfile("Player");
	const Vector3& AnimComponentMeshSize = m_Mesh->GetMeshSize();
	const Vector3& MeshRelativeScale = m_Mesh->GetRelativeScale();
	Vector3 ColliderCenter = Vector3(
		GetWorldPos().x,
		GetWorldPos().y + AnimComponentMeshSize.y * MeshRelativeScale.y * 0.5f,
		GetWorldPos().z
	);

	float ColliderRadiius = AnimComponentMeshSize.x * MeshRelativeScale.x;
	ColliderRadiius = AnimComponentMeshSize.y * MeshRelativeScale.y < ColliderRadiius ?
		AnimComponentMeshSize.y * MeshRelativeScale.y : ColliderRadiius;
	ColliderRadiius = AnimComponentMeshSize.z * MeshRelativeScale.z < ColliderRadiius ?
		AnimComponentMeshSize.z * MeshRelativeScale.z : ColliderRadiius;

	m_ColliderSphere->SetInfo(ColliderCenter, ColliderRadiius);
	*/

	// Picking 용 Box
	m_CullingArea3D = CreateComponent<CPickingLayerBox3D>("ColliderBox3D");
	m_Mesh->AddChild(m_CullingArea3D);
	m_CullingArea3D->SetLength(ColliderLength * 0.5f);

	// Weapon 달지 말고
	m_Weapon = m_Scene->CreateGameObject<CWeapon>("Weapon");
	m_Mesh->AddChild(m_Weapon, "Weapon");

	CInput::GetInst()->SetKeyCallback<CPlayer>("MoveFront", KeyState_Push,
		this, &CPlayer::MoveFront);
	CInput::GetInst()->SetKeyCallback<CPlayer>("MoveBack", KeyState_Push,
		this, &CPlayer::MoveBack);
	CInput::GetInst()->SetKeyCallback<CPlayer>("RotationYInv", KeyState_Push,
		this, &CPlayer::RotationYInv);
	CInput::GetInst()->SetKeyCallback<CPlayer>("RotationY", KeyState_Push,
		this, &CPlayer::RotationY);

	// 3차원 Nav Mesh
	// CInput::GetInst()->SetKeyCallback<CPlayer>("MovePoint", KeyState_Down,
	// 	this, &CPlayer::MovePoint);

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
			CInput::GetInst()->GetWheelDir() * 1.0f;

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

	bool PickResult = m_Scene->Picking(PickObj);

	if (PickResult)
	{
		// Picking 대상이 LandScape 라면, DDT 알고리즘을 이용한 이동 처리를 수행한다.
		// if (PickObj->GetRootComponent()->GetTypeID() != typeid(CLandScape).hash_code())
		// 	return;

		if (CInput::GetInst()->GetMouseLButtonClick())
		{
			// 오른쪽 클릭이 되었다면 해당 위치로 이동시킨다.
			// m_Scene->DDTPicking(PickObj, this);
			bool CheckResult = false;
		}
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
