
#include "Mesh.h"
#include "../../Device.h"
#include "../../PathManager.h"
#include "../../Scene/Scene.h"
#include "../../Scene/SceneResource.h"
#include "../ResourceManager.h"

#include "FBXLoader.h"

CMesh::CMesh()	:
	m_Scene(nullptr),
	m_Min(FLT_MAX, FLT_MAX, FLT_MAX),
	m_Max(FLT_MIN, FLT_MIN, FLT_MIN)
{
}

CMesh::~CMesh()
{
	size_t	Size = m_vecContainer.size();

	for (size_t i = 0; i < Size; ++i)
	{
		SAFE_DELETE(m_vecContainer[i]);
	}

	Size = m_vecMeshSlot.size();

	for (size_t i = 0; i < Size; ++i)
	{
		SAFE_DELETE(m_vecMeshSlot[i]);
	}
}

void CMesh::AddMaterialSlot(const std::string& Name)
{
	CMaterial* Material = nullptr;

	if (m_Scene)
	{
		Material = m_Scene->GetResource()->FindMaterial(Name);
	}

	else
	{
		Material = CResourceManager::GetInst()->FindMaterial(Name);
	}

	if (Material)
		m_vecMaterialSlot.push_back(Material);
}

bool CMesh::CreateMesh(void* VtxData, int Size, int Count, D3D11_USAGE Usage, D3D11_PRIMITIVE_TOPOLOGY Primitive,
	void* IdxData, int IdxSize, int IdxCount, D3D11_USAGE IdxUsage, DXGI_FORMAT Fmt)
{
	MeshContainer* Container = new MeshContainer;

	Container->VB.Size = Size;
	Container->VB.Count = Count;
	Container->Primitive = Primitive;

	if (!CreateBuffer(Buffer_Type::Vertex, VtxData, Size,
		Count, Usage, &Container->VB.Buffer))
		return false;

	Container->VB.Data = new char[Size * Count];
	memcpy(Container->VB.Data, VtxData, Size * Count);

	MeshSlot* Slot = new MeshSlot;

	m_vecMeshSlot.push_back(Slot);

	Slot->VB = &Container->VB;
	Slot->Primitive = Container->Primitive;

	if (IdxData != nullptr)
	{
		Container->vecIB.resize(1);

		Container->vecIB[0].Size = IdxSize;
		Container->vecIB[0].Count = IdxCount;
		Container->vecIB[0].Fmt = Fmt;

		if (!CreateBuffer(Buffer_Type::Index, IdxData, IdxSize,
			IdxCount, IdxUsage, &Container->vecIB[0].Buffer))
			return false;

		Container->vecIB[0].Data = new char[IdxSize * IdxCount];
		memcpy(Container->vecIB[0].Data, IdxData, IdxSize * IdxCount);

		Slot->IB = &Container->vecIB[0];
	}

	m_vecContainer.push_back(Container);


	return true;
}

bool CMesh::LoadMesh(const TCHAR* FileName, 
	const std::string& PathName)
{
	const PathInfo* Path = CPathManager::GetInst()->FindPath(PathName);

	TCHAR FullPath[MAX_PATH] = {};

	if (Path)
		lstrcpy(FullPath, Path->Path);

	lstrcat(FullPath, FileName);

	return LoadMeshFullPath(FullPath);
}

bool CMesh::LoadMeshFullPath(const TCHAR* FullPath)
{
	char    FullPathMultibyte[MAX_PATH] = {};

	int Length = WideCharToMultiByte(CP_ACP, 0, FullPath, -1, 0, 0, 0, 0);
	WideCharToMultiByte(CP_ACP, 0, FullPath, -1, FullPathMultibyte, Length, 0, 0);

	return LoadMeshFullPathMultibyte(FullPathMultibyte);
}

bool CMesh::LoadMeshMultibyte(const char* FileName,
	const std::string& PathName)
{
	const PathInfo* Path = CPathManager::GetInst()->FindPath(PathName);

	char FullPath[MAX_PATH] = {};

	if (Path)
		strcpy_s(FullPath, Path->PathMultibyte);

	strcat_s(FullPath, FileName);

	return LoadMeshFullPathMultibyte(FullPath);
}

bool CMesh::LoadMeshFullPathMultibyte(const char* FullPath)
{
	return false;
}

bool CMesh::Init()
{
	return true;
}

bool CMesh::CreateBuffer(Buffer_Type Type, void* Data, int Size,
	int Count, D3D11_USAGE Usage, ID3D11Buffer** Buffer)
{
	D3D11_BUFFER_DESC	Desc = {};
	
	Desc.ByteWidth = Size * Count;
	Desc.Usage = Usage;

	if (Type == Buffer_Type::Vertex)
		Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	else
		Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	if (Usage == D3D11_USAGE_DYNAMIC)
		Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	else if (Usage == D3D11_USAGE_STAGING)
		Desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA	BufferData = {};

	BufferData.pSysMem = Data;

	if (FAILED(CDevice::GetInst()->GetDevice()->CreateBuffer(&Desc,
		&BufferData, Buffer)))
		return false;


	if (Type == Buffer_Type::Vertex)
	{
		char* VertexData = (char*)Data;

		for (int i = 0; i < Count; ++i)
		{
			Vector3* Pos = (Vector3*)VertexData;
			VertexData += Size;

			if (m_Min.x > Pos->x)
				m_Min.x = Pos->x;

			if (m_Min.y > Pos->y)
				m_Min.y = Pos->y;

			if (m_Min.z > Pos->z)
				m_Min.z = Pos->z;

			if (m_Max.x < Pos->x)
				m_Max.x = Pos->x;

			if (m_Max.y < Pos->y)
				m_Max.y = Pos->y;

			if (m_Max.z < Pos->z)
				m_Max.z = Pos->z;
		}
	}

	return true;
}

bool CMesh::SaveMeshFile(const char* FullPath)
{
	FILE* File = nullptr;

	fopen_s(&File, FullPath, "wb");

	if (!File)
		return false;

	SaveMesh(File);

	fclose(File);

	return true;
}

bool CMesh::LoadMeshFile(const char* FullPath)
{
	FILE* File = nullptr;

	fopen_s(&File, FullPath, "rb");

	if (!File)
		return false;

	LoadMesh(File);

	fclose(File);

	return true;
}

bool CMesh::ConvertFBX(CFBXLoader* Loader, const char* FullPath)
{
	const std::vector<std::vector<PFBXMATERIAL>>* vecMaterials = Loader->GetMaterials();
	const std::vector<PFBXMESHCONTAINER>* vecContainers = Loader->GetMeshContainers();

	auto    iter = vecContainers->begin();
	auto    iterEnd = vecContainers->end();

	bool    BumpEnable = false;
	bool    AnimationEnable = false;

	std::vector<std::vector<bool>>  vecEmptyIndex;

	int ContainerIndex = 0;

	for (; iter != iterEnd; ++iter, ++ContainerIndex)
	{
		MeshContainer* Container = new MeshContainer;

		m_vecContainer.push_back(Container);

		std::vector<bool>    vecEmpty;
		vecEmptyIndex.push_back(vecEmpty);

		if ((*iter)->Bump)
			BumpEnable = true;

		if ((*iter)->Animation)
			AnimationEnable = true;

		std::vector<Vertex3D>   vecVtx;

		size_t  VtxCount = (*iter)->vecPos.size();

		vecVtx.resize(VtxCount);

		for (size_t i = 0; i < VtxCount; ++i)
		{
			Vertex3D    Vtx = {};

			Vtx.Pos = (*iter)->vecPos[i];
			Vtx.Normal = (*iter)->vecNormal[i];
			Vtx.UV = (*iter)->vecUV[i];

			if (BumpEnable)
			{
				if (!(*iter)->vecTangent.empty())
					Vtx.Tangent = (*iter)->vecTangent[i];

				if (!(*iter)->vecBinormal.empty())
					Vtx.Binormal = (*iter)->vecBinormal[i];
			}

			if (!(*iter)->vecBlendWeight.empty() && AnimationEnable)
			{
				Vtx.BlendWeight = (*iter)->vecBlendWeight[i];
				Vtx.BlendIndex = (*iter)->vecBlendIndex[i];
			}

			vecVtx[i] = Vtx;
		}

		// Mesh의 Vertex 생성
		if (!CreateBuffer(Buffer_Type ::Vertex,
			&vecVtx[0], sizeof(Vertex3D), (int)vecVtx.size(),
			D3D11_USAGE_DEFAULT, &Container->VB.Buffer))
			return false;

		Container->VB.Size = sizeof(Vertex3D);
		Container->VB.Count = (int)vecVtx.size();
		Container->VB.Data = new char[Container->VB.Size * Container->VB.Count];
		memcpy(Container->VB.Data, &vecVtx[0], Container->VB.Size * Container->VB.Count);

		Container->Primitive = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		size_t  IdxCount = (*iter)->vecIndices.size();

		for (size_t i = 0; i < IdxCount; ++i)
		{
			// 인덱스 버퍼를 채워줄 데이터가 없다면 해당 서브셋은 폐기한다.
			if ((*iter)->vecIndices[i].empty())
			{
				vecEmptyIndex[ContainerIndex].push_back(false);
				continue;
			}

			vecEmptyIndex[ContainerIndex].push_back(true);

			MeshSlot* Slot = new MeshSlot;

			m_vecMeshSlot.push_back(Slot);

			Slot->VB = &Container->VB;
			Slot->Primitive = Container->Primitive;
			
			IndexBuffer IdxBuffer = {};

			Container->vecIB.push_back(IdxBuffer);

			if (!CreateBuffer(Buffer_Type::Index,
				&(*iter)->vecIndices[i][0], sizeof(UINT),
				(int)(*iter)->vecIndices[i].size(),
				D3D11_USAGE_DEFAULT, &Container->vecIB[i].Buffer))
				return false;

			Container->vecIB[i].Size = sizeof(UINT);
			Container->vecIB[i].Count = (int)(*iter)->vecIndices[i].size();
			Container->vecIB[i].Fmt = DXGI_FORMAT_R32_UINT;
			Container->vecIB[i].Data = new char[Container->vecIB[i].Size * Container->vecIB[i].Count];

			memcpy(Container->vecIB[i].Data, &(*iter)->vecIndices[i][0],
				Container->vecIB[i].Size * Container->vecIB[i].Count);

			Slot->IB = &Container->vecIB[i];
		}
	}


	// 재질 정보를 읽어온다.
	auto    iterM = vecMaterials->begin();
	auto    iterMEnd = vecMaterials->end();

	ContainerIndex = 0;

	for (; iterM != iterMEnd; ++iterM, ++ContainerIndex)
	{
		// 서브셋 수만큼 반복한다.
		size_t  Size = (*iterM).size();

		for (size_t i = 0; i < Size; ++i)
		{
			if (!vecEmptyIndex[ContainerIndex][i])
				continue;

			PFBXMATERIAL    Mtrl = (*iterM)[i];

			CMaterial* Material = new CMaterial;

			Material->CreateConstantBuffer();

			Material->SetBaseColor(Mtrl->BaseColor);
			Material->SetAmbientColor(Mtrl->Amb);
			Material->SetSpecularColor(Mtrl->Spc);
			Material->SetEmissiveColor(Mtrl->Emv);
			Material->SetSpecularPower(Mtrl->Shininess);

			Material->SetShader("Standard3DShader");

			AddMaterialSlot(Material);

			// Texture
			char    FileName[MAX_PATH] = {};
			_splitpath_s(Mtrl->DifTex.c_str(), 0, 0, 0, 0, FileName, MAX_PATH, 0, 0);

			TCHAR   FullPath[MAX_PATH] = {};

#ifdef UNICODE
			int PathLength = MultiByteToWideChar(CP_ACP, 0, Mtrl->DifTex.c_str(),
				-1, 0, 0);
			MultiByteToWideChar(CP_ACP, 0, Mtrl->DifTex.c_str(), -1, FullPath, PathLength);
#else
			strcpy_s(FullPath, Mtrl->DifTex.c_str());
#endif // UNICODE

			Material->AddTextureFullPath(0, (int)Buffer_Shader_Type::Pixel,
				FileName, FullPath);

			if (!Mtrl->BumpTex.empty())
			{
				Material->EnableBump();
				memset(FileName, 0, MAX_PATH);
				_splitpath_s(Mtrl->BumpTex.c_str(), 0, 0, 0, 0, FileName, MAX_PATH, 0, 0);

				memset(FullPath, 0, sizeof(TCHAR) * MAX_PATH);

#ifdef UNICODE
				int PathLength = MultiByteToWideChar(CP_ACP, 0, Mtrl->BumpTex.c_str(),
					-1, 0, 0);
				MultiByteToWideChar(CP_ACP, 0, Mtrl->BumpTex.c_str(), -1, FullPath, PathLength);
#else
				strcpy_s(FullPath, Mtrl->BumpTex.c_str());
#endif // UNICODE

				Material->AddTextureFullPath(1, (int)Buffer_Shader_Type::Pixel,
					FileName, FullPath);
			}

			if (!Mtrl->SpcTex.empty())
			{
				memset(FileName, 0, MAX_PATH);
				_splitpath_s(Mtrl->SpcTex.c_str(), 0, 0, 0, 0, FileName, MAX_PATH, 0, 0);

				memset(FullPath, 0, sizeof(TCHAR) * MAX_PATH);

#ifdef UNICODE
				int PathLength = MultiByteToWideChar(CP_ACP, 0, Mtrl->SpcTex.c_str(),
					-1, 0, 0);
				MultiByteToWideChar(CP_ACP, 0, Mtrl->SpcTex.c_str(), -1, FullPath, PathLength);
#else
				strcpy_s(FullPath, Mtrl->SpcTex.c_str());
#endif // UNICODE

				Material->AddTextureFullPath(2, (int)Buffer_Shader_Type::Pixel,
					FileName, FullPath);

				Material->EnableSpecularTex();
			}

			if (BumpEnable)
				Material->EnableBump();

			if (AnimationEnable)
				Material->EnableAnimation3D();
		}
	}


	// 자체포맷으로 저장해준다.
	char    MeshFullPath[MAX_PATH] = {};

	strcpy_s(MeshFullPath, FullPath);
	int PathLength = (int)strlen(FullPath);
	memcpy(&MeshFullPath[PathLength - 3], "msh", 3);

	SaveMeshFile(MeshFullPath);

	return true;
}

bool CMesh::SaveMesh(FILE* File)
{
	fwrite(&m_MeshType, sizeof(Mesh_Type), 1, File);

	fwrite(&m_Min, sizeof(Vector3), 1, File);
	fwrite(&m_Max, sizeof(Vector3), 1, File);

	int	Length = (int)m_Name.length();

	fwrite(&Length, sizeof(int), 1, File);
	fwrite(m_Name.c_str(), sizeof(char), Length, File);

	int	ContainerCount = (int)m_vecContainer.size();
	fwrite(&ContainerCount, sizeof(int), 1, File);

	for (int i = 0; i < ContainerCount; ++i)
	{
		MeshContainer* Container = m_vecContainer[i];


		fwrite(&Container->Primitive, sizeof(D3D11_PRIMITIVE_TOPOLOGY), 1, File);

		fwrite(&Container->VB.Size, sizeof(int), 1, File);
		fwrite(&Container->VB.Count, sizeof(int), 1, File);
		fwrite(Container->VB.Data, Container->VB.Size, 
			Container->VB.Count, File);

		int	IdxCount = (int)Container->vecIB.size();

		fwrite(&IdxCount, sizeof(int), 1, File);

		for (int j = 0; j < IdxCount; ++j)
		{
			fwrite(&Container->vecIB[j].Size, sizeof(int), 1, File);
			fwrite(&Container->vecIB[j].Count, sizeof(int), 1, File);
			fwrite(&Container->vecIB[j].Fmt, sizeof(DXGI_FORMAT), 1, File);
			fwrite(Container->vecIB[j].Data, Container->vecIB[j].Size,
				Container->vecIB[j].Count, File);
		}
	}

	int MaterialCount = (int)m_vecMaterialSlot.size();

	fwrite(&MaterialCount, sizeof(int), 1, File);

	for (int i = 0; i < MaterialCount; ++i)
	{
		m_vecMaterialSlot[i]->Save(File);
	}


	return true;
}

bool CMesh::LoadMesh(FILE* File)
{
	fread(&m_MeshType, sizeof(Mesh_Type), 1, File);

	fread(&m_Min, sizeof(Vector3), 1, File);
	fread(&m_Max, sizeof(Vector3), 1, File);

	int	Length = 0;

	char	Name[256] = {};

	fread(&Length, sizeof(int), 1, File);
	fread(Name, sizeof(char), Length, File);

	m_Name = Name;

	int	ContainerCount = 0;
	fread(&ContainerCount, sizeof(int), 1, File);

	for (int i = 0; i < ContainerCount; ++i)
	{
		MeshContainer* Container = new MeshContainer;

		m_vecContainer.push_back(Container);


		fread(&Container->Primitive, sizeof(D3D11_PRIMITIVE_TOPOLOGY), 1, File);

		fread(&Container->VB.Size, sizeof(int), 1, File);
		fread(&Container->VB.Count, sizeof(int), 1, File);

		Container->VB.Data = new char[Container->VB.Size * Container->VB.Count];

		fread(Container->VB.Data, Container->VB.Size,
			Container->VB.Count, File);

		if (!CreateBuffer(Buffer_Type::Vertex,
			Container->VB.Data, Container->VB.Size, Container->VB.Count,
			D3D11_USAGE_DEFAULT, &Container->VB.Buffer))
			return false;

		int	IdxCount = 0;

		fread(&IdxCount, sizeof(int), 1, File);

		Container->vecIB.resize(IdxCount);

		for (int j = 0; j < IdxCount; ++j)
		{
			fread(&Container->vecIB[j].Size, sizeof(int), 1, File);
			fread(&Container->vecIB[j].Count, sizeof(int), 1, File);
			fread(&Container->vecIB[j].Fmt, sizeof(DXGI_FORMAT), 1, File);

			Container->vecIB[j].Data = new char[Container->vecIB[j].Size * Container->vecIB[j].Count];

			fread(Container->vecIB[j].Data, Container->vecIB[j].Size,
				Container->vecIB[j].Count, File);

			if (!CreateBuffer(Buffer_Type::Index,
				Container->vecIB[j].Data, Container->vecIB[j].Size,
				Container->vecIB[j].Count,
				D3D11_USAGE_DEFAULT, &Container->vecIB[j].Buffer))
				return false;

			MeshSlot* Slot = new MeshSlot;

			m_vecMeshSlot.push_back(Slot);

			Slot->VB = &Container->VB;
			Slot->Primitive = Container->Primitive;

			Slot->IB = &Container->vecIB[j];
		}
	}

	int MaterialCount = 0;

	fread(&MaterialCount, sizeof(int), 1, File);

	for (int i = 0; i < MaterialCount; ++i)
	{
		CMaterial* Material = new CMaterial;

		m_vecMaterialSlot.push_back(Material);

		Material->Load(File);
	}

	return true;
}

void CMesh::Render()
{
	size_t	Size = m_vecContainer.size();

	for (size_t i = 0; i < Size; ++i)
	{
		unsigned int	Stride = (unsigned int)m_vecContainer[i]->VB.Size;
		unsigned int	Offset = 0;

		CDevice::GetInst()->GetContext()->IASetPrimitiveTopology(m_vecContainer[i]->Primitive);
		CDevice::GetInst()->GetContext()->IASetVertexBuffers(0, 1,
			&m_vecContainer[i]->VB.Buffer, &Stride, &Offset);

		size_t	IdxCount = m_vecContainer[i]->vecIB.size();

		if (IdxCount > 0)
		{
			for (size_t j = 0; j < IdxCount; ++j)
			{
				CDevice::GetInst()->GetContext()->IASetIndexBuffer(
					m_vecContainer[i]->vecIB[j].Buffer,
					m_vecContainer[i]->vecIB[j].Fmt, 0);
				CDevice::GetInst()->GetContext()->DrawIndexed(
					m_vecContainer[i]->vecIB[j].Count, 0, 0);
			}
		}

		else
		{
			CDevice::GetInst()->GetContext()->IASetIndexBuffer(
				nullptr, DXGI_FORMAT_UNKNOWN, 0);
			CDevice::GetInst()->GetContext()->Draw(
				m_vecContainer[i]->VB.Count, 0);
		}
	}
}

void CMesh::Render(int SlotNumber)
{
	unsigned int	Stride = (unsigned int)m_vecMeshSlot[SlotNumber]->VB->Size;
	unsigned int	Offset = 0;

	CDevice::GetInst()->GetContext()->IASetPrimitiveTopology(m_vecMeshSlot[SlotNumber]->Primitive);
	CDevice::GetInst()->GetContext()->IASetVertexBuffers(0, 1,
		&m_vecMeshSlot[SlotNumber]->VB->Buffer, &Stride, &Offset);

	if (m_vecMeshSlot[SlotNumber]->IB)
	{
		CDevice::GetInst()->GetContext()->IASetIndexBuffer(
			m_vecMeshSlot[SlotNumber]->IB->Buffer,
			m_vecMeshSlot[SlotNumber]->IB->Fmt, 0);
		CDevice::GetInst()->GetContext()->DrawIndexed(
			m_vecMeshSlot[SlotNumber]->IB->Count, 0, 0);
	}

	else
	{
		CDevice::GetInst()->GetContext()->IASetIndexBuffer(
			nullptr, DXGI_FORMAT_UNKNOWN, 0);
		CDevice::GetInst()->GetContext()->Draw(
			m_vecMeshSlot[SlotNumber]->VB->Count, 0);
	}
}

void CMesh::RenderInstancing(int Count)
{
	size_t	Size = m_vecContainer.size();

	for (size_t i = 0; i < Size; ++i)
	{
		unsigned int	Stride = (unsigned int)m_vecContainer[i]->VB.Size;
		unsigned int	Offset = 0;

		CDevice::GetInst()->GetContext()->IASetPrimitiveTopology(m_vecContainer[i]->Primitive);
		CDevice::GetInst()->GetContext()->IASetVertexBuffers(0, 1,
			&m_vecContainer[i]->VB.Buffer, &Stride, &Offset);

		size_t	IdxCount = m_vecContainer[i]->vecIB.size();

		if (IdxCount > 0)
		{
			for (size_t j = 0; j < IdxCount; ++j)
			{
				CDevice::GetInst()->GetContext()->IASetIndexBuffer(
					m_vecContainer[i]->vecIB[j].Buffer,
					m_vecContainer[i]->vecIB[j].Fmt, 0);
				CDevice::GetInst()->GetContext()->DrawIndexedInstanced(
					m_vecContainer[i]->vecIB[j].Count, Count, 0, 0, 0);
			}
		}

		else
		{
			CDevice::GetInst()->GetContext()->IASetIndexBuffer(
				nullptr, DXGI_FORMAT_UNKNOWN, 0);
			CDevice::GetInst()->GetContext()->DrawInstanced(
				m_vecContainer[i]->VB.Count, Count, 0, 0);
		}
	}
}

void CMesh::RenderInstancing(int Count, int SlotNumber)
{
	unsigned int	Stride = (unsigned int)m_vecMeshSlot[SlotNumber]->VB->Size;
	unsigned int	Offset = 0;

	CDevice::GetInst()->GetContext()->IASetPrimitiveTopology(m_vecMeshSlot[SlotNumber]->Primitive);
	CDevice::GetInst()->GetContext()->IASetVertexBuffers(0, 1,
		&m_vecMeshSlot[SlotNumber]->VB->Buffer, &Stride, &Offset);

	if (m_vecMeshSlot[SlotNumber]->IB)
	{
		CDevice::GetInst()->GetContext()->IASetIndexBuffer(
			m_vecMeshSlot[SlotNumber]->IB->Buffer,
			m_vecMeshSlot[SlotNumber]->IB->Fmt, 0);
		CDevice::GetInst()->GetContext()->DrawIndexedInstanced(
			m_vecMeshSlot[SlotNumber]->IB->Count, Count, 0, 0, 0);
	}

	else
	{
		CDevice::GetInst()->GetContext()->IASetIndexBuffer(
			nullptr, DXGI_FORMAT_UNKNOWN, 0);
		CDevice::GetInst()->GetContext()->DrawInstanced(
			m_vecMeshSlot[SlotNumber]->VB->Count, Count, 0, 0);
	}
}

void CMesh::RenderInstancing(ID3D11Buffer* InstancingBuffer, unsigned int InstanceSize, int Count)
{
	size_t	Size = m_vecContainer.size();

	for (size_t i = 0; i < Size; ++i)
	{
		unsigned int	Stride[2] = { (unsigned int)m_vecContainer[i]->VB.Size, InstanceSize };
		unsigned int	Offset[2] = {};

		ID3D11Buffer* Buffer[2] = { m_vecContainer[i]->VB.Buffer , InstancingBuffer };

		CDevice::GetInst()->GetContext()->IASetPrimitiveTopology(m_vecContainer[i]->Primitive);
		CDevice::GetInst()->GetContext()->IASetVertexBuffers(0, 2,
			Buffer, Stride, Offset);

		size_t	IdxCount = m_vecContainer[i]->vecIB.size();

		if (IdxCount > 0)
		{
			for (size_t j = 0; j < IdxCount; ++j)
			{
				CDevice::GetInst()->GetContext()->IASetIndexBuffer(
					m_vecContainer[i]->vecIB[j].Buffer,
					m_vecContainer[i]->vecIB[j].Fmt, 0);
				CDevice::GetInst()->GetContext()->DrawIndexedInstanced(
					m_vecContainer[i]->vecIB[j].Count, Count, 0, 0, 0);
			}
		}

		else
		{
			CDevice::GetInst()->GetContext()->IASetIndexBuffer(
				nullptr, DXGI_FORMAT_UNKNOWN, 0);
			CDevice::GetInst()->GetContext()->DrawInstanced(
				m_vecContainer[i]->VB.Count, Count, 0, 0);
		}
	}
}

void CMesh::RenderInstancing(ID3D11Buffer* InstancingBuffer,
	unsigned int InstanceSize, int Count, int SlotNumber)
{
	unsigned int	Stride[2] = { (unsigned int)m_vecMeshSlot[SlotNumber]->VB->Size, InstanceSize };
	unsigned int	Offset[2] = {};

	ID3D11Buffer* Buffer[2] = { m_vecMeshSlot[SlotNumber]->VB->Buffer , InstancingBuffer };

	CDevice::GetInst()->GetContext()->IASetPrimitiveTopology(m_vecMeshSlot[SlotNumber]->Primitive);
	CDevice::GetInst()->GetContext()->IASetVertexBuffers(0, 2,
		Buffer, Stride, Offset);

	if (m_vecMeshSlot[SlotNumber]->IB)
	{
		CDevice::GetInst()->GetContext()->IASetIndexBuffer(
			m_vecMeshSlot[SlotNumber]->IB->Buffer,
			m_vecMeshSlot[SlotNumber]->IB->Fmt, 0);
		CDevice::GetInst()->GetContext()->DrawIndexedInstanced(
			m_vecMeshSlot[SlotNumber]->IB->Count, Count, 0, 0, 0);
	}

	else
	{
		CDevice::GetInst()->GetContext()->IASetIndexBuffer(
			nullptr, DXGI_FORMAT_UNKNOWN, 0);
		CDevice::GetInst()->GetContext()->DrawInstanced(
			m_vecMeshSlot[SlotNumber]->VB->Count, Count, 0, 0);
	}
}
