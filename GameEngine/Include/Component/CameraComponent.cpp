
#include "CameraComponent.h"
#include "../Device.h"
#include "Frustum.h"
#include "../Scene/Scene.h"
#include "../Scene/LightManager.h"
#include "LightComponent.h"
#include "../Render/RenderManager.h"
#include "Arm.h"

CCameraComponent::CCameraComponent()
{
	SetTypeID<CCameraComponent>();
	m_Render = false;

	m_CameraType = Camera_Type::Camera3D;
	m_ViewAngle = 90.f;
	m_Distance = 1000.f;

	m_Frustum = new CFrustum;
}

CCameraComponent::CCameraComponent(const CCameraComponent& com) :
	CSceneComponent(com)
{
	m_CameraType = com.m_CameraType;
	m_matView = com.m_matView;
	m_matProj = com.m_matProj;
	m_ViewAngle = com.m_ViewAngle;
	m_Distance = com.m_Distance;
	m_RS = com.m_RS;

	m_Frustum = new CFrustum;
}

CCameraComponent::~CCameraComponent()
{
	SAFE_DELETE(m_Frustum);
}

void CCameraComponent::CreateProjectionMatrix()
{
	switch (m_CameraType)
	{
	case Camera_Type::Camera2D:
		m_matProj = XMMatrixOrthographicOffCenterLH(0.f, (float)m_RS.Width, 0.f, (float)m_RS.Height, 0.f, 1000.f);
		break;
	case Camera_Type::Camera3D:
		m_matProj = XMMatrixPerspectiveFovLH(DegreeToRadian(m_ViewAngle),
			m_RS.Width / (float)m_RS.Height, 0.1f, m_Distance);
		break;
	case Camera_Type::CameraUI:
		m_matProj = XMMatrixOrthographicOffCenterLH(0.f, (float)m_RS.Width, 0.f, (float)m_RS.Height, 0.f, 1000.f);
		break;
	}

	m_matShadowProj = XMMatrixPerspectiveFovLH(DegreeToRadian(m_ViewAngle),
		SHADOWMAP_WIDTH / SHADOWMAP_HEIGHT, 0.1f, m_Distance);
	//m_matShadowProj = XMMatrixOrthographicLH((float)m_RS.Width, (float)m_RS.Height, 0.f, 1000.f);
}

bool CCameraComponent::FrustumInPoint(const Vector3& Point)
{
	return m_Frustum->FrustumInPoint(Point);
}

bool CCameraComponent::FrustumInSphere(const SphereInfo& Sphere)
{
	return m_Frustum->FrustumInSphere(Sphere);
}

void CCameraComponent::ComputeShadowView()
{
	CLightComponent* GlobalLight = m_Scene->GetLightManager()->GetGlobalLightComponent();

	m_matShadowView.Identity();

	for (int i = 0; i < AXIS_MAX; ++i)
	{
		Vector3	Axis = GlobalLight->GetWorldAxis((AXIS)i);
		memcpy(&m_matShadowView[i][0], &Axis, sizeof(Vector3));
	}

	m_matShadowView.Transpose();

	Vector3	TargetPos;

	if (m_Parent->CheckType<CArm>())
	{
		CArm* Parent = (CArm*)m_Parent;

		TargetPos = GetWorldPos() + GetWorldAxis(AXIS_Z) * Parent->GetTargetDistance();
	}

	float ShadowLightDistance = CRenderManager::GetInst()->GetShadowLightDistance();

	Vector3	Pos = TargetPos - GlobalLight->GetWorldAxis(AXIS_Z) * ShadowLightDistance;

	Pos *= -1.f;

	for (int i = 0; i < AXIS_MAX; ++i)
	{
		Vector3	Axis = GlobalLight->GetWorldAxis((AXIS)i);

		m_matShadowView[3][i] = Pos.Dot(Axis);
	}
}

void CCameraComponent::Start()
{
	CSceneComponent::Start();

	CreateProjectionMatrix();
}

bool CCameraComponent::Init()
{
	m_RS = CDevice::GetInst()->GetResolution();

	return true;
}

void CCameraComponent::Update(float DeltaTime)
{
	CSceneComponent::Update(DeltaTime);

	/*
	카메라가 뷰공간으로 변환이 되면 카메라를 구성하는 x, y, z 축은 월드의 축과 일치하게
	된다. 즉, 축은 x : 1, 0, 0  y : 0, 1, 0  z : 0, 0, 1 로 구성이 된다는 것이다.
	직교행렬 : 행렬을 구성하는 3개의 축이 모두 서로에 대해 정직교 하는 행렬을 말한다.
	1 0 0
	0 1 0
	0 0 1

	직교행렬은 전치 행렬과 역행렬이 같다.

	카메라 X축 * 뷰행렬 = 1, 0, 0
	카메라 y축 * 뷰행렬 = 0, 1, 0
	카메라 z축 * 뷰행렬 = 0, 0, 1
	x, y, z   0 0 0   1 0 0
	x, y, z * 0 0 0 = 0 1 0
	x, y, z	  0 0 0   0 0 1

	뷰의 회전 행렬
	Xx Yx Zx 0
	Xy Yy Zy 0
	Xz Yz Zz 0
	0  0  0  1

	뷰의 이동 행렬
	1   0  0 0
	0   1  0 0
	0   0  1 0
	-x -y -z 1

	1   0  0 0   Xx Yx Zx 0
	0   1  0 0 * Xy Yy Zy 0
	0   0  1 0   Xz Yz Zz 0
	-x -y -z 1   0  0  0  1

	최종 뷰 행렬
	Xx    Yx   Zx  0
	Xy    Yy   Zy  0
	Xz    Yz   Zz  0
	-X.P -Y.P -Z.P 1
	*/	
}

void CCameraComponent::PostUpdate(float DeltaTime)
{
	CSceneComponent::PostUpdate(DeltaTime);

	m_matView.Identity();

	/*
	i가 0일때
	Xx Xy Xz 0
	0  1  0  0
	0  0  1  0
	0  0  0  1

	i가 1일때
	Xx Xy Xz 0
	Yx Yy Yz 0
	0  0  1  0
	0  0  0  1

	i가 2일때
	Xx Xy Xz 0
	Yx Yy Yz 0
	Zx Zy Zz 0
	0  0  0  1
	*/
	for (int i = 0; i < AXIS_MAX; ++i)
	{
		Vector3	Axis = GetWorldAxis((AXIS)i);
		memcpy(&m_matView[i][0], &Axis, sizeof(Vector3));
	}

	/*
	Xx Xy Xz 0
	Yx Yy Yz 0
	Zx Zy Zz 0
	0  0  0  1
	 
	전치하면
	Xx Yx Zx 0
	Xy Yy Zy 0
	Xz Yz Zz 0
	0  0  0  1
	*/
	m_matView.Transpose();

	Vector3	Pos = GetWorldPos() * -1.f;

	/*
	Xx    Yx   Zx  0
	Xy    Yy   Zy  0
	Xz    Yz   Zz  0
	-X.P -Y.P -Z.P 1
	*/
	for (int i = 0; i < AXIS_MAX; ++i)
	{
		Vector3	Axis = GetWorldAxis((AXIS)i);

		m_matView[3][i] = Pos.Dot(Axis);
	}

	m_Frustum->Update(m_matView * m_matProj);
}

void CCameraComponent::PrevRender()
{
	CSceneComponent::PrevRender();
}

void CCameraComponent::Render()
{
	CSceneComponent::Render();
}

void CCameraComponent::PostRender()
{
	CSceneComponent::PostRender();
}

CCameraComponent* CCameraComponent::Clone()
{
	return new CCameraComponent(*this);
}

void CCameraComponent::Save(FILE* File)
{
	fwrite(&m_CameraType, sizeof(Camera_Type), 1, File);
	fwrite(&m_matView, sizeof(Matrix), 1, File);
	fwrite(&m_matProj, sizeof(Matrix), 1, File);
	fwrite(&m_ViewAngle, sizeof(float), 1, File);
	fwrite(&m_Distance, sizeof(float), 1, File);
	fwrite(&m_RS, sizeof(Resolution), 1, File);
	
	CSceneComponent::Save(File);
}

void CCameraComponent::Load(FILE* File)
{
	fread(&m_CameraType, sizeof(Camera_Type), 1, File);
	fread(&m_matView, sizeof(Matrix), 1, File);
	fread(&m_matProj, sizeof(Matrix), 1, File);
	fread(&m_ViewAngle, sizeof(float), 1, File);
	fread(&m_Distance, sizeof(float), 1, File);
	fread(&m_RS, sizeof(Resolution), 1, File);

	CSceneComponent::Load(File);
}
