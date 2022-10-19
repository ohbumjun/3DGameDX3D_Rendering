#include "WaterComponent.h"
#include "PathManager.h"
#include "Resource/Mesh/StaticMesh.h"
#include "Scene/Scene.h"
#include "Scene/Navigation3DManager.h"
#include "Scene/SceneResource.h"
#include "Resource/Material/Material.h"
#include "Resource/Shader/WaterConstantBuffer.h"

CWaterComponent::CWaterComponent() :
	m_CBuffer(nullptr),
	m_Min(FLT_MAX, FLT_MAX, FLT_MAX),
	m_Max(FLT_MIN, FLT_MIN, FLT_MIN)
{
	SetTypeID<CWaterComponent>();
	m_Render = true;
	
	m_LayerName = "Water";
	// m_LayerName = "Transparent";
}

CWaterComponent::CWaterComponent(const CWaterComponent& com) :
	CSceneComponent(com)
{
	m_Mesh = com.m_Mesh;
	m_CBuffer = com.m_CBuffer->Clone();
}

CWaterComponent::~CWaterComponent()
{
	SAFE_DELETE(m_CBuffer);
}

void CWaterComponent::CreateWaterMesh(const std::string& Name, int CountX, int CountZ)
{
	m_CountX = CountX;
	m_CountZ = CountZ;

	std::vector<float>	vecY;
	vecY.resize(CountX * CountZ);

	// 위치 정보 세팅 (Pos, UV)
	for (unsigned int i = 0; i < m_CountZ; ++i)
	{
		for (unsigned int j = 0; j < m_CountX; ++j)
		{
			Vertex3D	Vtx = {};

			// 위에서 아래로 내려오면서 계산한다고 생각하면 된다.
			Vtx.Pos = Vector3((float)j, vecY[i * m_CountX + j], (float)m_CountZ - i - 1);
			// Vtx.UV = Vector2(j / (float)(m_CountX - 1), i / (float)m_CountZ - 1);
			Vtx.UV = Vector2(j / (float)m_CountX, 1 - ((i + 1) / (float)m_CountZ));

			m_vecPos.push_back(Vtx.Pos);


			if (m_Min.x > Vtx.Pos.x)
				m_Min.x = Vtx.Pos.x;

			if (m_Min.y > Vtx.Pos.y)
				m_Min.y = Vtx.Pos.y;

			if (m_Min.z > Vtx.Pos.z)
				m_Min.z = Vtx.Pos.z;

			if (m_Max.x < Vtx.Pos.x)
				m_Max.x = Vtx.Pos.x;

			if (m_Max.y < Vtx.Pos.y)
				m_Max.y = Vtx.Pos.y;

			if (m_Max.z < Vtx.Pos.z)
				m_Max.z = Vtx.Pos.z;

			m_vecVtx.push_back(Vtx);
		}
	}

	SetMeshSize(m_Max - m_Min);

	m_SphereInfo.Center = (m_Max + m_Min) / 2.f;

	// 인덱스 정보 만들기
	// 면 법선은 삼각형 수만큼 만들어져야 한다.
	// 그리고 밖을 향하게 하기 위해서 시계 방향으로 주어지게 해야 한다.
	Vector3	TrianglePos[3];
	Vector3	Edge1, Edge2;
	int	TriIndex = 0;

	m_vecFaceNormal.resize((m_CountX - 1) * (m_CountZ - 1) * 2);

	for (unsigned i = 0; i < m_CountZ - 1; ++i)
	{
		for (unsigned j = 0; j < m_CountX - 1; ++j)
		{
			unsigned int	Index = i * m_CountX + j;

			// 사각형의 우 상단 삼각형
			m_vecIndex.push_back(Index);
			m_vecIndex.push_back(Index + 1);
			m_vecIndex.push_back(Index + m_CountX + 1);

			TrianglePos[0] = m_vecVtx[Index].Pos;
			TrianglePos[1] = m_vecVtx[Index + 1].Pos;
			TrianglePos[2] = m_vecVtx[Index + m_CountX + 1].Pos;

			Edge1 = TrianglePos[1] - TrianglePos[0];
			Edge2 = TrianglePos[2] - TrianglePos[0];

			Edge1.Normalize();
			Edge2.Normalize();

			m_vecFaceNormal[TriIndex] = Edge1.Cross(Edge2);
			m_vecFaceNormal[TriIndex].Normalize();

			++TriIndex;


			// 사각형의 좌 하단 삼각형
			m_vecIndex.push_back(Index);
			m_vecIndex.push_back(Index + m_CountX + 1);
			m_vecIndex.push_back(Index + m_CountX);

			TrianglePos[0] = m_vecVtx[Index].Pos;
			TrianglePos[1] = m_vecVtx[Index + m_CountX + 1].Pos;
			TrianglePos[2] = m_vecVtx[Index + m_CountX].Pos;

			Edge1 = TrianglePos[1] - TrianglePos[0];
			Edge2 = TrianglePos[2] - TrianglePos[0];

			Edge1.Normalize();
			Edge2.Normalize();

			m_vecFaceNormal[TriIndex] = Edge1.Cross(Edge2);
			m_vecFaceNormal[TriIndex].Normalize();

			++TriIndex;
		}
	}

	ComputeNormal();

	ComputeTangent();

	// 메쉬를 만든다.
	m_Scene->GetResource()->CreateMesh(Mesh_Type::Static, Name,
		&m_vecVtx[0], sizeof(Vertex3D), (int)m_vecVtx.size(),
		D3D11_USAGE_DEFAULT, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		&m_vecIndex[0], sizeof(int), (int)m_vecIndex.size(),
		D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT);

	m_Mesh = (CStaticMesh*)m_Scene->GetResource()->FindMesh(Name);
}

void CWaterComponent::SetMesh(const std::string& Name)
{
	// m_Mesh = (CSprite3DMesh*)m_Scene->GetResource()->FindMesh(Name);
}

void CWaterComponent::SetMaterial(CMaterial* Material, int Index)
{
	m_Material = Material->Clone();

	m_Material->SetScene(m_Scene);
}

void CWaterComponent::SetMaterial(const std::string& Name, int Index)
{
	CMaterial* Material = m_Scene->GetResource()->FindMaterial(Name);

	m_Material = Material->Clone();

	m_Material->SetScene(m_Scene); //
}


void CWaterComponent::SetWaveHeight(float WaveHeight)
{
	m_CBuffer->SetWaveHeight(WaveHeight);
}
void CWaterComponent::SetWaveSpeed(float WaveSpeed)
{
	m_CBuffer->SetWaveSpeed(WaveSpeed);
}
void CWaterComponent::SetWaveFrequency(float WaveFrequencey)
{
	m_CBuffer->SetWaveFrequency(WaveFrequencey);
}

void CWaterComponent::Start()
{
	CSceneComponent::Start();
}

bool CWaterComponent::Init()
{
	m_CBuffer = new CWaterConstantBuffer;

	m_CBuffer->Init();

	// m_Mesh = (CSprite3DMesh*)m_Scene->GetResource()->FindMesh("Sprite3DMesh");
	CreateWaterMesh("WaterMesh", 40, 40);
	// m_Mesh = (CSpriteMesh*)m_Scene->GetResource()->FindMesh("SpriteMesh");

	m_Material = m_Scene->GetResource()->FindMaterial("WaterMaterial");
	// m_Material = m_Scene->GetResource()->FindMaterial("BillBoard");

	SetPivot(0.5f, 0.5f, 0.f);
	SetRelativeScale(1.f, 1.f, 1.f);
	SetMeshSize(1.f, 1.f, 1.f);
	// SetMeshSize(1.f, 1.f, 1.f);

	m_SkyMaterial = m_Scene->GetResource()->FindMaterial("SkyMaterial");

	return true;
}

void CWaterComponent::Update(float DeltaTime)
{
	CSceneComponent::Update(DeltaTime);
}

void CWaterComponent::PostUpdate(float DeltaTime)
{
	CSceneComponent::PostUpdate(DeltaTime);
}

void CWaterComponent::PrevRender()
{
	CSceneComponent::PrevRender();
}

void CWaterComponent::Render()
{
	CSceneComponent::Render();

	if (!m_Mesh)
		return;

	m_CBuffer->UpdateCBuffer();

	// Sky Material
	// m_SkyMaterial->Render();

	// Water Material
	m_Material->Render();

	m_Mesh->Render();

	m_Material->Reset();

	// m_SkyMaterial->Reset();
}


void CWaterComponent::PostRender()
{
	CSceneComponent::PostRender();
}

CWaterComponent* CWaterComponent::Clone()
{
	return new CWaterComponent(*this);
}

void CWaterComponent::Save(FILE* File)
{
	CSceneComponent::Save(File);
}

void CWaterComponent::Load(FILE* File)
{
	CSceneComponent::Load(File);
}

void CWaterComponent::ComputeNormal()
{
	size_t	Size = m_vecFaceNormal.size();

	for (size_t i = 0; i < Size; ++i)
	{
		unsigned int	Idx0 = m_vecIndex[i * 3];
		unsigned int	Idx1 = m_vecIndex[i * 3 + 1];
		unsigned int	Idx2 = m_vecIndex[i * 3 + 2];

		// 삼각형의 Normal 을 -> 삼각형 각 정점의 Normal 로 세팅해준다.
		m_vecVtx[Idx0].Normal += m_vecFaceNormal[i];
		m_vecVtx[Idx1].Normal += m_vecFaceNormal[i];
		m_vecVtx[Idx2].Normal += m_vecFaceNormal[i];
	}

	Size = m_vecVtx.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecVtx[i].Normal.Normalize();
	}
}

void CWaterComponent::ComputeTangent()
{
	// 탄젠트 벡터 구함.
	for (size_t i = 0; i < m_vecFaceNormal.size(); ++i)
	{
		int	idx0 = m_vecIndex[i * 3];
		int	idx1 = m_vecIndex[i * 3 + 1];
		int	idx2 = m_vecIndex[i * 3 + 2];

		float	fVtx1[3], fVtx2[3];
		fVtx1[0] = m_vecVtx[idx1].Pos.x - m_vecVtx[idx0].Pos.x;
		fVtx1[1] = m_vecVtx[idx1].Pos.y - m_vecVtx[idx0].Pos.y;
		fVtx1[2] = m_vecVtx[idx1].Pos.z - m_vecVtx[idx0].Pos.z;

		fVtx2[0] = m_vecVtx[idx2].Pos.x - m_vecVtx[idx0].Pos.x;
		fVtx2[1] = m_vecVtx[idx2].Pos.y - m_vecVtx[idx0].Pos.y;
		fVtx2[2] = m_vecVtx[idx2].Pos.z - m_vecVtx[idx0].Pos.z;

		float	ftu[2], ftv[2];
		ftu[0] = m_vecVtx[idx1].UV.x - m_vecVtx[idx0].UV.x;
		ftv[0] = m_vecVtx[idx1].UV.y - m_vecVtx[idx0].UV.y;

		ftu[1] = m_vecVtx[idx2].UV.x - m_vecVtx[idx0].UV.x;
		ftv[1] = m_vecVtx[idx2].UV.y - m_vecVtx[idx0].UV.y;

		float	fDen = 1.f / (ftu[0] * ftv[1] - ftu[1] * ftv[0]);

		Vector3	Tangent;
		Tangent.x = (ftv[1] * fVtx1[0] - ftv[0] * fVtx2[0]) * fDen;
		Tangent.y = (ftv[1] * fVtx1[1] - ftv[0] * fVtx2[1]) * fDen;
		Tangent.z = (ftv[1] * fVtx1[2] - ftv[0] * fVtx2[2]) * fDen;

		Tangent.Normalize();

		m_vecVtx[idx0].Tangent = Tangent;
		m_vecVtx[idx1].Tangent = Tangent;
		m_vecVtx[idx2].Tangent = Tangent;

		m_vecVtx[idx0].Binormal = m_vecVtx[idx0].Normal.Cross(Tangent);
		m_vecVtx[idx1].Binormal = m_vecVtx[idx1].Normal.Cross(Tangent);
		m_vecVtx[idx2].Binormal = m_vecVtx[idx2].Normal.Cross(Tangent);

		m_vecVtx[idx0].Binormal.Normalize();
		m_vecVtx[idx1].Binormal.Normalize();
		m_vecVtx[idx2].Binormal.Normalize();
	}
}
