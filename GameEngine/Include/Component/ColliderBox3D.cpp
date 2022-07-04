#include "ColliderBox3D.h"
#include "../Collision/Collision.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneResource.h"
#include "../Resource/Shader/ColliderConstantBuffer.h"
#include "../Scene/Scene.h"
#include "../Scene/CameraManager.h"
#include "CameraComponent.h"
#include "AnimationMeshComponent.h"
#include "ColliderCircle.h"
#include "ColliderPixel.h"

CColliderBox3D::CColliderBox3D()
{
	SetTypeID<CColliderBox3D>();
	m_ComponentType = Component_Type::SceneComponent;
	m_Render = true;

	m_ColliderType = Collider_Type::Box3D;

	// DebugLayer 에 출력
	m_LayerName = "DebugLayer";
}

CColliderBox3D::CColliderBox3D(const CColliderBox3D& com)
{
	m_Info = com.m_Info;
}

CColliderBox3D::~CColliderBox3D()
{
}

void CColliderBox3D::Start()
{
	CColliderComponent::Start();
}

bool CColliderBox3D::Init()
{
	if (!CColliderComponent::Init())
		return false;

	m_Info.Axis[0] = Vector3(1.f, 0.f, 0.f);
	m_Info.Axis[1] = Vector3(0.f, 1.f, 0.f);
	m_Info.Axis[2] = Vector3(0.f, 0.f, 1.f);

	// Length : 한변 * 0.5 ?
	m_Info.Length = Vector3(5.f, 5.f, 5.f);

	SetInheritRotX(true);
	SetInheritRotY(true);
	SetInheritRotZ(true);

	SetWorldScale(m_Info.Length.x * 2.f, m_Info.Length.y * 2.f, m_Info.Length.z * 2.f);

	m_Mesh = m_Scene->GetResource()->FindMesh("CubeLinePos");
}

void CColliderBox3D::Update(float DeltaTime)
{
	CColliderComponent::Update(DeltaTime);
}

void CColliderBox3D::PostUpdate(float DeltaTime)
{
	CColliderComponent::PostUpdate(DeltaTime);

	if (m_Object->GetRootComponent()->GetTypeID() == typeid(CAnimationMeshComponent).hash_code())
		bool True = true;

	m_Info.Center.x = GetWorldPos().x + m_Offset.x;
	m_Info.Center.y = GetWorldPos().y + m_Offset.y;
	m_Info.Center.z = GetWorldPos().z + m_Offset.z;

	m_Info.Axis[0].x = GetWorldAxis(AXIS_X).x;
	m_Info.Axis[0].y = GetWorldAxis(AXIS_X).y;
	m_Info.Axis[0].z = GetWorldAxis(AXIS_X).z;

	m_Info.Axis[1].x = GetWorldAxis(AXIS_Y).x;
	m_Info.Axis[1].y = GetWorldAxis(AXIS_Y).y;
	m_Info.Axis[1].z = GetWorldAxis(AXIS_Y).z;

	m_Info.Axis[2].x = GetWorldAxis(AXIS_Z).x;
	m_Info.Axis[2].y = GetWorldAxis(AXIS_Z).y;
	m_Info.Axis[2].z = GetWorldAxis(AXIS_Z).z;

	Vector3	Pos[8] = {};

	// 전면
	Pos[0] = m_Info.Center - m_Info.Axis[0] * m_Info.Length.x + m_Info.Axis[1] * m_Info.Length.y - m_Info.Axis[2] * m_Info.Length.z;  // 왼상
	Pos[1] = m_Info.Center + m_Info.Axis[0] * m_Info.Length.x + m_Info.Axis[1] * m_Info.Length.y - m_Info.Axis[2] * m_Info.Length.z; // 오상
	Pos[2] = m_Info.Center - m_Info.Axis[0] * m_Info.Length.x - m_Info.Axis[1] * m_Info.Length.y - m_Info.Axis[2] * m_Info.Length.z;   // 왼하
	Pos[3] = m_Info.Center + m_Info.Axis[0] * m_Info.Length.x - m_Info.Axis[1] * m_Info.Length.y - m_Info.Axis[2] * m_Info.Length.z;  // 오하

	// 후면
	Pos[4] = m_Info.Center - m_Info.Axis[0] * m_Info.Length.x + m_Info.Axis[1] * m_Info.Length.y + m_Info.Axis[2] * m_Info.Length.z;
	Pos[5] = m_Info.Center + m_Info.Axis[0] * m_Info.Length.x + m_Info.Axis[1] * m_Info.Length.y + m_Info.Axis[2] * m_Info.Length.z;
	Pos[6] = m_Info.Center - m_Info.Axis[0] * m_Info.Length.x - m_Info.Axis[1] * m_Info.Length.y + m_Info.Axis[2] * m_Info.Length.z;
	Pos[7] = m_Info.Center + m_Info.Axis[0] * m_Info.Length.x - m_Info.Axis[1] * m_Info.Length.y + m_Info.Axis[2] * m_Info.Length.z;

	m_Min.x = Pos[0].x;
	m_Min.y = Pos[0].y;
	m_Min.z = Pos[0].z;

	m_Max.x = Pos[0].x;
	m_Max.y = Pos[0].y;
	m_Max.z = Pos[0].z;

	for (int i = 1; i < 8; ++i)
	{
		if (m_Min.x > Pos[i].x)
			m_Min.x = Pos[i].x;

		if (m_Min.y > Pos[i].y)
			m_Min.y = Pos[i].y;

		if (m_Min.z > Pos[i].z)
			m_Min.z = Pos[i].z;

		if (m_Max.x < Pos[i].x)
			m_Max.x = Pos[i].x;

		if (m_Max.y < Pos[i].y)
			m_Max.y = Pos[i].y;

		if (m_Max.z < Pos[i].z)
			m_Max.z = Pos[i].z;
	}

	m_Info.Min.x = m_Min.x;
	m_Info.Min.y = m_Min.y;
	m_Info.Min.z = m_Min.z;

	m_Info.Max.x = m_Max.x;
	m_Info.Max.y = m_Max.y;
	m_Info.Max.z = m_Max.z;
}

void CColliderBox3D::PrevRender()
{
	CColliderComponent::PrevRender();
}

void CColliderBox3D::Render()
{
	CColliderComponent::Render();

	CCameraComponent* Camera = m_Scene->GetCameraManager()->GetCurrentCamera();

	Matrix	matWorld, matView, matProj, matWVP;

	matView = Camera->GetViewMatrix();
	matProj = Camera->GetProjMatrix();

	Matrix	matScale, matRot, matTrans;

	matScale.Scaling(m_Info.Length.x * 2.f, m_Info.Length.y * 2.f, m_Info.Length.z * 2.f);
	matRot.Rotation(GetWorldRot());

	if (m_Object->GetRootComponent()->GetTypeID() == typeid(CAnimationMeshComponent).hash_code())
		bool True = true;

	matTrans.Translation(m_Info.Center);

	matWorld = matScale * matRot * matTrans;

	matWVP = matWorld * matView * matProj;

	matWVP.Transpose();

	m_CBuffer->SetWVP(matWVP);

	if (m_PrevCollisionList.empty())
		m_CBuffer->SetColliderColor(Vector4(0.f, 1.f, 0.f, 1.f));

	else
		m_CBuffer->SetColliderColor(Vector4(1.f, 0.f, 0.f, 1.f));

	if (m_MouseCollision)
		m_CBuffer->SetColliderColor(Vector4(1.f, 0.f, 0.f, 1.f));

	// Culling 영역 표시용이라면 
	if (m_LayerName == "PickingCullingLayer")
		m_CBuffer->SetColliderColor(Vector4(0.f, 0.f, 1.f, 1.f));

	m_CBuffer->UpdateCBuffer();

	m_Shader->SetShader();

	m_Mesh->Render();

}

void CColliderBox3D::PostRender()
{
	CColliderComponent::PostRender();
}

CColliderBox3D* CColliderBox3D::Clone()
{
	return nullptr;
}

void CColliderBox3D::Save(FILE* File)
{
}

void CColliderBox3D::Load(FILE* File)
{
}

bool CColliderBox3D::Collision(CColliderComponent* Dest)
{
	switch (Dest->GetColliderType())
	{
	case Collider_Type::Box3D:
		return CCollision::CollisionBox3DToBox3D((CColliderBox3D*)Dest, this);
	case Collider_Type::Sphere:
		return CCollision::CollisionSphereToBox3D((CColliderSphere*)Dest, this);
	}

	return false;
}

bool CColliderBox3D::CollisionMouse(const Vector2& MousePos)
{
	return m_MouseCollision;
}

bool CColliderBox3D::CollisionRay(const Ray& ray)
{
	return false;
}
