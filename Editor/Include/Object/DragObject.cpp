
#include "DragObject.h"
#include "Component/StaticMeshComponent.h"
#include "Animation/AnimationSequence2DInstance.h"

CDragObject::CDragObject()
{
}

CDragObject::CDragObject(const CDragObject& obj) :
	CGameObject(obj)
{
	m_Mesh = (CStaticMeshComponent*)FindComponent("Mesh");
}

CDragObject::~CDragObject()
{
}

void CDragObject::SetStartPos(const Vector2& Pos)
{
	m_StartPos = Pos;
	m_Mesh->SetWorldPos(m_StartPos.x, m_StartPos.y, 0.f);
}

void CDragObject::SetEndPos(const Vector2& Pos)
{
	m_EndPos = Pos;

	m_Mesh->SetWorldScale(m_EndPos.x - m_StartPos.x, m_EndPos.y - m_StartPos.y, 1.f);
}

bool CDragObject::Init()
{
	m_Mesh = CreateComponent<CStaticMeshComponent>("Mesh");

	SetRootComponent(m_Mesh);

	m_Mesh->SetLayerName("DragLayer");
	m_Mesh->SetMesh("FrameRect");
	m_Mesh->GetMaterial()->SetShader("PosMeshShader");
	m_Mesh->SetPivot(0.f, 1.f, 0.f);
	m_Mesh->SetBaseColor(1.f, 0.f, 1.f, 1.f);

	return true;
}

void CDragObject::Update(float DeltaTime)
{
	CGameObject::Update(DeltaTime);
}

void CDragObject::PostUpdate(float DeltaTime)
{
	CGameObject::PostUpdate(DeltaTime);
}

CDragObject* CDragObject::Clone()
{
	return new CDragObject(*this);
}
