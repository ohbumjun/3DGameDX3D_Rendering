#include "PickingLayerBox3D.h"
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

CPickingLayerBox3D::CPickingLayerBox3D()
{
	SetTypeID<CPickingLayerBox3D>();
	m_ComponentType = Component_Type::SceneComponent;
	m_Render = true;

	// DebugLayer 에 출력
	m_LayerName = "PickingCullingLayer";
}

CPickingLayerBox3D::CPickingLayerBox3D(const CPickingLayerBox3D& com)
{
	m_Info = com.m_Info;
}

CPickingLayerBox3D::~CPickingLayerBox3D()
{
	SAFE_DELETE(m_CBuffer);
}

void CPickingLayerBox3D::Start()
{
	CSceneComponent::Start();
}

bool CPickingLayerBox3D::Init()
{
	if (!CSceneComponent::Init())
		return false;

	m_CBuffer = new CColliderConstantBuffer;

	m_CBuffer->Init();

	m_CBuffer->SetColliderColor(Vector4(0.f, 1.f, 0.f, 1.f));

	m_Shader = CResourceManager::GetInst()->FindShader("ColliderShader");

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

void CPickingLayerBox3D::Update(float DeltaTime)
{
	CSceneComponent::Update(DeltaTime);
}

void CPickingLayerBox3D::PostUpdate(float DeltaTime)
{
	CSceneComponent::PostUpdate(DeltaTime);

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

void CPickingLayerBox3D::PrevRender()
{
	CSceneComponent::PrevRender();
}

void CPickingLayerBox3D::Render()
{
	CSceneComponent::Render();

	CCameraComponent* Camera = m_Scene->GetCameraManager()->GetCurrentCamera();

	Matrix	matWorld, matView, matProj, matWVP;

	matView = Camera->GetViewMatrix();
	matProj = Camera->GetProjMatrix();

	Matrix	matScale, matRot, matTrans;

	matScale.Scaling(m_Info.Length.x * 2.f, m_Info.Length.y * 2.f, m_Info.Length.z * 2.f);
	matRot.Rotation(GetWorldRot());

	// LandScape Component 의 PikcingLayer
	// if (m_Object->GetRootComponent()->GetTypeID() == typeid(CLandScape).hash_code())
	if (m_Object->GetRootComponent()->GetTypeID() == typeid(CAnimationMeshComponent).hash_code())
		bool True = true;

	// Sphere Info 의 Center 영역으로 세팅한다.
	SphereInfo WorldSphereInfo = m_Object->GetRootComponent()->GetSphereInfo();
	// matTrans.Translation(m_Info.Center);
	matTrans.Translation(WorldSphereInfo.Center); 

	matWorld = matScale * matRot * matTrans;

	matWVP = matWorld * matView * matProj;

	matWVP.Transpose();

	m_CBuffer->SetWVP(matWVP);

	m_CBuffer->SetColliderColor(Vector4(0.f, 0.f, 1.f, 1.f));

	m_CBuffer->UpdateCBuffer();

	m_Shader->SetShader();

	m_Mesh->Render();

}

void CPickingLayerBox3D::PostRender()
{
	CSceneComponent::PostRender();
}
