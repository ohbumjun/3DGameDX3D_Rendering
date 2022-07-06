
#include "LandScape.h"
#include "../PathManager.h"
#include "../Resource/Mesh/StaticMesh.h"
#include "../Scene/Scene.h"
#include "../Scene/Navigation3DManager.h"
#include "../Scene/SceneResource.h"
#include "../Resource/Material/Material.h"
#include "../Resource/Shader/LandScapeConstantBuffer.h"
#include "../Scene/Navigation3D.h"
#include "../Scene/NavigationMesh.h"

CLandScape::CLandScape()	:
	m_CountX(0),
	m_CountZ(0),
	m_CBuffer(nullptr),
	m_Min(FLT_MAX, FLT_MAX, FLT_MAX),
	m_Max(FLT_MIN, FLT_MIN, FLT_MIN)
{
	SetTypeID<CLandScape>();
	m_Render = true;
}

CLandScape::CLandScape(const CLandScape& com)	:
	CSceneComponent(com)
{
	m_CountX = com.m_CountX;
	m_CountZ = com.m_CountZ;
	m_Mesh = com.m_Mesh;
	m_vecVtx = com.m_vecVtx;
	m_vecPos = com.m_vecPos;
	m_vecIndex = com.m_vecIndex;

	m_CBuffer = com.m_CBuffer->Clone();
}

CLandScape::~CLandScape()
{
	SAFE_DELETE(m_CBuffer);
}

// 가져온 Height Map 에 기반하여
// 격자 개수만큼의 정점을 지닌 
// Mesh 를 만들어주는 함수
// 각 정점의 위치, Normal, Index, Tagnent, Binormal 정보를 모두 만들어준다.
void CLandScape::CreateLandScape(const std::string& Name,
	int CountX, int CountZ, const TCHAR* HeightMap, 
	const std::string& PathName)
{
	m_CountX = CountX;
	m_CountZ = CountZ;

	std::vector<float>	vecY;

	if (HeightMap)
	{
		TCHAR	FullPath[MAX_PATH] = {};

		const PathInfo* Path = CPathManager::GetInst()->FindPath(PathName);

		if (Path)
			lstrcpy(FullPath, Path->Path);
		lstrcat(FullPath, HeightMap);

		ScratchImage	Img;

		char	Ext[_MAX_EXT] = {};
		char	FullPathMultibyte[MAX_PATH] = {};

#ifdef UNICODE

		int	ConvertLength = WideCharToMultiByte(CP_ACP, 0, FullPath, -1, nullptr, 0, nullptr, nullptr);
		WideCharToMultiByte(CP_ACP, 0, FullPath, -1, FullPathMultibyte, ConvertLength, nullptr, nullptr);

#else

		strcpy_s(FullPathMultibyte, FullPath);

#endif // UNICODE

		_splitpath_s(FullPathMultibyte, nullptr, 0, nullptr, 0, nullptr, 0, Ext, _MAX_EXT);

		_strupr_s(Ext);

		if (strcmp(Ext, ".DDS") == 0)
		{
			if (FAILED(LoadFromDDSFile(FullPath, DDS_FLAGS_NONE, nullptr, Img)))
			{
				return;
			}
		}

		else if (strcmp(Ext, ".TGA") == 0)
		{
			if (FAILED(LoadFromTGAFile(FullPath, nullptr, Img)))
			{
				return;
			}
		}

		else
		{
			if (FAILED(LoadFromWICFile(FullPath, WIC_FLAGS_NONE, nullptr, Img)))
			{
				return;
			}
		}

		// 픽셀 값을 꺼내온다.
		const Image* PixelInfo = Img.GetImages();

		for (size_t i = 0; i < PixelInfo->height; ++i)
		{
			for (size_t j = 0; j < PixelInfo->width; ++j)
			{
				// 픽셀은 1byte 4개짜리가 모여서 하나의 색상을 만든다.
				// 따라서 Idx * 4 를 해줘야 한다.
				int PixelIndex = (int)i * (int)PixelInfo->width * 4 + (int)j * 4;

				// 이렇게 하면, r,g,b,a 중에서 r 값만 뽑아올 것이다.
				// Height Map 은 Gray Scale 형태로 뽑아올 것이다.
				// 따라서 r,g,b,a 중 어떤 것을 뽑아와도 상관없다.
				// 다시 말하면,애초에 Height Map 자체게, Gray Scale 이고
				// 해당 Pixel 의 r,g,b,a 값은, 원래 높이 정보들을 r,g,b,a 에 동일하게 저장한 형태이므로
				// GrayScale 형태를 띨 수 밖에 없다는 것이다.

				// 그리고 GrayScale 에 저장된 픽셀은 0 에서 255사이의 값이다 (r,g,b,a 자체가 0에서 255 사이의 값)
				// 그런데, 실제 Render 시에 높이 255 는 너무 큰값이다.
				// 따라서, 60으로 나눠줄 것이다 ( 마음대로 세팅해도 되는 값)
				float	Y = PixelInfo->pixels[PixelIndex] / 30.f;

				vecY.push_back(Y);
			}
		}
	}

	// 만약 Height Map 이 없다면, 
	else
		vecY.resize(CountX * CountZ);

	for (int i = 0; i < m_CountZ; ++i)
	{
		for (int j = 0; j < m_CountX; ++j)
		{
			// 위치, UV 정보를 세팅해준다.
			Vertex3D	Vtx = {};

			// x,y,z
			// 왼쪽 위에서부터 내려오므로 i 가 아니라 m_CountZ - i 
			// ex) 1 ~ 100 --> 0 ~ 99 로 만들기 위함 --> 따라서 - 1
			Vtx.Pos = Vector3((float)j, vecY[i * m_CountX + j],
				(float)m_CountZ - i - 1);

			// UV 의 경우, ex) 0 ~ 99 로 x가 들어갈 때 --> 99일 때 1 이라는 값이 나와야 한다.
			Vtx.UV = Vector2(j / (float)(m_CountX - 1),
				i / (float)m_CountZ - 1);

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
	Vector3	TrianglePos[3];
	Vector3	Edge1, Edge2;
	unsigned int	TriIndex = 0;

	// 삼각형 개수만큼 만들기 
	// 왜 - 1을 해줘서 곱해주는 것인가 ?
	// ex) 5*5 격자 --> 실제 만들어지는 사각형은 4*4 -> 16개
	// ex) N개의 사각형 --> N * 2개의 삼각형 을 순회하면서 색인 버퍼 정보를 만들어줘야 한다.
	m_vecFaceNormal.resize((m_CountX - 1) * (m_CountZ - 1) * 2);

	for (unsigned int i = 0; i < m_CountZ - 1; ++i)
	{
		for (unsigned int j = 0; j < m_CountX - 1; ++j)
		{
			unsigned int	Index = i * m_CountX + j;

			// 사각형의 우상단 삼각형 (시계방향 순서로 정점 정보를 지정해준다. -> 전면 삼각형 형태로 만들기 위하여)
			// Idx Buffer 를 만들어주는 과정
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

			// 해당 삼각형의 법선 벡터 정보를 만들어준다.
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

	m_Scene->GetNavigation3DManager()->AddNavData(this);
}

void CLandScape::SetMaterial(CMaterial* Material, int Index)
{
	m_vecMaterialSlot[Index] = Material->Clone();

	m_vecMaterialSlot[Index]->SetScene(m_Scene);
}

void CLandScape::SetMaterial(const std::string& Name, int Index)
{
	CMaterial* Material = m_Scene->GetResource()->FindMaterial(Name);

	m_vecMaterialSlot[Index] = Material->Clone();

	m_vecMaterialSlot[Index]->SetScene(m_Scene);
}

void CLandScape::AddMaterial(CMaterial* Material)
{
	m_vecMaterialSlot.push_back(Material->Clone());

	m_vecMaterialSlot[m_vecMaterialSlot.size() - 1]->SetScene(m_Scene);
}

void CLandScape::AddMaterial(const std::string& Name)
{
	CMaterial* Material = m_Scene->GetResource()->FindMaterial(Name);

	m_vecMaterialSlot.push_back(Material->Clone());

	m_vecMaterialSlot[m_vecMaterialSlot.size() - 1]->SetScene(m_Scene);
}

void CLandScape::SetDetailLevel(float Level)
{
	m_CBuffer->SetDetailLevel(Level);
}

void CLandScape::SetSplatCount(int Count)
{
	m_CBuffer->SetSplatCount(Count);
}

void CLandScape::CreateNavigationData(CNavigationMesh* NavMesh)
{
	float	CellSizeX = 0.f, CellSizeZ = 0.f;

	CellSizeX = m_vecVtx[1].Pos.x - m_vecVtx[0].Pos.x;
	CellSizeZ = m_vecVtx[0].Pos.z - m_vecVtx[m_CountX].Pos.z;

	Vector3	CellPos[3];

	for (int i = 0; i < m_CountZ - 1; ++i)
	{
		for (int j = 0; j < m_CountX - 1; ++j)
		{
			UINT	Index = i * m_CountX + j;

			CellPos[0] = m_vecVtx[Index].Pos;
			CellPos[1] = m_vecVtx[Index + 1].Pos;
			CellPos[2] = m_vecVtx[Index + m_CountX + 1].Pos;

			NavMesh->AddCell(CellPos);

			// 좌하단 삼각형
			CellPos[0] = m_vecVtx[Index].Pos;
			CellPos[1] = m_vecVtx[Index + m_CountX + 1].Pos;
			CellPos[2] = m_vecVtx[Index + m_CountX].Pos;

			NavMesh->AddCell(CellPos);
		}
	}

	NavMesh->SetCellSize(CellSizeX, CellSizeZ);

	NavMesh->CreateGridMapAdj(m_CountX - 1);
}

float CLandScape::GetHeight(const Vector3& Pos)
{
	Vector3	Convert = (Pos - GetWorldPos()) / GetWorldScale();

	// z 좌표 역으로 계산
	Convert.z = m_CountZ - 1 - Convert.z;

	int	IndexX = (int)Convert.x;
	int	IndexZ = (int)Convert.z;

	if (IndexX < 0 || IndexX >= m_CountX || IndexZ < 0 || IndexZ >= m_CountZ - 1)
		return Pos.y;

	int	Index = IndexZ * m_CountX + IndexX;

	float	RatioX = Convert.x - IndexX;
	float	RatioZ = Convert.z - IndexZ;

	float	Y[4] =
	{
		m_vecPos[Index].y,
		m_vecPos[Index + 1].y,
		m_vecPos[Index + m_CountX].y,
		m_vecPos[Index + m_CountX + 1].y
	};

	// 우상단 삼각형
	if (RatioX > RatioZ)
		return Y[0] + (Y[1] - Y[0]) * RatioX + (Y[3] - Y[1]) * RatioZ;

	return Y[0] + (Y[3] - Y[2]) * RatioX + (Y[2] - Y[0]) * RatioZ;
}

void CLandScape::Start()
{
	CSceneComponent::Start();
}

bool CLandScape::Init()
{
	m_CBuffer = new CLandScapeConstantBuffer;

	m_CBuffer->Init();

	return true;
}

void CLandScape::Update(float DeltaTime)
{
	CSceneComponent::Update(DeltaTime);
}

void CLandScape::PostUpdate(float DeltaTime)
{
	CSceneComponent::PostUpdate(DeltaTime);
}

void CLandScape::PrevRender()
{
	CSceneComponent::PrevRender();
}

void CLandScape::Render()
{
	CSceneComponent::Render();

	if (!m_Mesh)
		return;

	m_CBuffer->UpdateCBuffer();

	size_t	Size = m_vecMaterialSlot.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecMaterialSlot[i]->EnableDecal(m_ReceiveDecal);

		m_vecMaterialSlot[i]->Render();

		m_Mesh->Render((int)i);

		m_vecMaterialSlot[i]->Reset();
	}
}

void CLandScape::RenderShadowMap()
{
	CSceneComponent::RenderShadowMap();

	if (!m_Mesh)
		return;

	m_CBuffer->UpdateCBuffer();

	size_t	Size = m_vecMaterialSlot.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_Mesh->Render((int)i);
	}
}

void CLandScape::PostRender()
{
	CSceneComponent::PostRender();
}

CLandScape* CLandScape::Clone()
{
	return new CLandScape(*this);
}

void CLandScape::Save(FILE* File)
{
	CSceneComponent::Save(File);
}

void CLandScape::Load(FILE* File)
{
	CSceneComponent::Load(File);

	m_Scene->GetNavigation3DManager()->AddNavData(this);
}

void CLandScape::ComputeNormal()
{
	// 전체 삼각형 개수 만큼 반복을 돌린다.
	size_t	Size = m_vecFaceNormal.size();

	// 이제 각 삼각형의 법선벡터 정보를 이용해서, 
	// 각 정점에서의 법선벡터 정보를 만들어낼 것이다.
	// 특정 정점의 법선벡터는, 해당 정점이 구성하는 면들의 법선 벡터값 평균을 내면 된다.
	for (size_t i = 0; i < Size; ++i)
	{
		// i번째 면을 구성하는 3개의 정점 정보를 불러온다.
		unsigned int	Idx0 = m_vecIndex[i * 3];
		unsigned int	Idx1 = m_vecIndex[i * 3 + 1];
		unsigned int	Idx2 = m_vecIndex[i * 3 + 2];

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

void CLandScape::ComputeTangent()
{
	// 탄젠트 벡터 구함.
	// 삼각형 개수만큼 순회한다.
	for (size_t i = 0; i < m_vecFaceNormal.size(); ++i)
	{
		// 특정 삼각형의 3개 정점 정보를 불러온다.
		int	idx0 = m_vecIndex[i * 3];
		int	idx1 = m_vecIndex[i * 3 + 1];
		int	idx2 = m_vecIndex[i * 3 + 2];

		// 위치 정보
		float	fVtx1[3], fVtx2[3];
		fVtx1[0] = m_vecVtx[idx1].Pos.x - m_vecVtx[idx0].Pos.x;
		fVtx1[1] = m_vecVtx[idx1].Pos.y - m_vecVtx[idx0].Pos.y;
		fVtx1[2] = m_vecVtx[idx1].Pos.z - m_vecVtx[idx0].Pos.z;

		fVtx2[0] = m_vecVtx[idx2].Pos.x - m_vecVtx[idx0].Pos.x;
		fVtx2[1] = m_vecVtx[idx2].Pos.y - m_vecVtx[idx0].Pos.y;
		fVtx2[2] = m_vecVtx[idx2].Pos.z - m_vecVtx[idx0].Pos.z;

		// 법선 벡터 정보
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
