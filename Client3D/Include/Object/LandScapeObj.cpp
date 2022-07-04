
#include "LandScapeObj.h"
#include "Component/PickingLayerBox3D.h"

CLandScapeObj::CLandScapeObj()
{
	SetTypeID<CLandScapeObj>();
}

CLandScapeObj::CLandScapeObj(const CLandScapeObj& obj) :
	CGameObject(obj)
{
	m_LandScape	 = (CLandScape*)FindComponent("LandScape");
}

CLandScapeObj::~CLandScapeObj()
{
}

bool CLandScapeObj::Init()
{
	m_LandScape = CreateComponent<CLandScape>("LandScape");

	// 129, 129 sms, Height Map 의 크기 에 따라서 세팅해준 값이다.
	m_LandScape->CreateLandScape("LandScape", 129, 129,
		TEXT("LandScape/height1.bmp"));

	m_LandScape->AddMaterial("LandScape");

	m_LandScape->SetDetailLevel(30.f);
	m_LandScape->SetSplatCount(4);

	// Culling Area
	const Vector3& AnimComponentMeshSize = m_LandScape->GetMeshSize();
	const Vector3& MeshRelativeScale = m_LandScape->GetRelativeScale();

	// 아래 수치는 정석 (하지만 실제 크기는 이에 맞게 조정해줘야 한다)
	// Vector3 ColliderLength = Vector3(
	// 	AnimComponentMeshSize.x * MeshRelativeScale.x,
	// 	AnimComponentMeshSize.y * MeshRelativeScale.y,
	// 	AnimComponentMeshSize.z * MeshRelativeScale.z
	// );
	Vector3 ColliderLength = Vector3(
		AnimComponentMeshSize.x * MeshRelativeScale.x,
		AnimComponentMeshSize.y * MeshRelativeScale.y,
		AnimComponentMeshSize.z * MeshRelativeScale.z
	);

	m_CullingArea3D = CreateComponent<CPickingLayerBox3D>("ColliderBox3D");
	m_LandScape->AddChild(m_CullingArea3D);
	m_CullingArea3D->SetLength(ColliderLength * 0.5f);

	return true;
}

void CLandScapeObj::Update(float DeltaTime)
{
	CGameObject::Update(DeltaTime);
}

void CLandScapeObj::PostUpdate(float DeltaTime)
{
	CGameObject::PostUpdate(DeltaTime);
}

CLandScapeObj* CLandScapeObj::Clone()
{
	return new CLandScapeObj(*this);
}
