
#include "Arm.h"
#include "../Device.h"

CArm::CArm()
{
	SetTypeID<CArm>();
	m_Render = false;

	m_TargetDistance = 5.f;
}

CArm::CArm(const CArm& com) :
	CSceneComponent(com)
{
	m_TargetDistance = com.m_TargetDistance;
	m_Offset = com.m_Offset;
}

CArm::~CArm()
{
}

void CArm::Start()
{
	CSceneComponent::Start();
}

bool CArm::Init()
{
	return true;
}

void CArm::Update(float DeltaTime)
{
	CSceneComponent::Update(DeltaTime);
	
	if (m_Parent)
	{
		Vector3	ParentPos = m_Parent->GetWorldPos();

		Vector3	Pos = ParentPos - GetWorldAxis(AXIS_Z) * m_TargetDistance;

		SetWorldPos(Pos + m_Offset);
	}
}

void CArm::PostUpdate(float DeltaTime)
{
	CSceneComponent::PostUpdate(DeltaTime);
}

void CArm::PrevRender()
{
	CSceneComponent::PrevRender();
}

void CArm::Render()
{
	CSceneComponent::Render();
}

void CArm::PostRender()
{
	CSceneComponent::PostRender();
}

CArm* CArm::Clone()
{
	return new CArm(*this);
}

void CArm::Save(FILE* File)
{
	fwrite(&m_TargetDistance, sizeof(float), 1, File);
	fwrite(&m_Offset, sizeof(Vector3), 1, File);

	CSceneComponent::Save(File);
}

void CArm::Load(FILE* File)
{
	fread(&m_TargetDistance, sizeof(float), 1, File);
	fread(&m_Offset, sizeof(Vector3), 1, File);

	CSceneComponent::Load(File);
}
