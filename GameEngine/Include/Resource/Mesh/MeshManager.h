#pragma once

#include "../../GameInfo.h"

class CMeshManager
{
	friend class CResourceManager;

private:
	CMeshManager();
	~CMeshManager();

private:
	std::unordered_map<std::string, CSharedPtr<class CMesh>>	m_mapMesh;

public:
	bool Init();
	bool CreateMesh(Mesh_Type Type, const std::string& Name,
		void* VtxData, int Size, int Count, D3D11_USAGE Usage,
		D3D11_PRIMITIVE_TOPOLOGY Primitive, void* IdxData = nullptr, int IdxSize = 0, int IdxCount = 0,
		D3D11_USAGE IdxUsage = D3D11_USAGE_DEFAULT, DXGI_FORMAT Fmt = DXGI_FORMAT_UNKNOWN,
		class CScene* Scene = nullptr);
	bool LoadMesh(Mesh_Type Type, const std::string& Name, const TCHAR* FileName,
		const std::string& PathName = MESH_PATH, class CScene* Scene = nullptr);
	bool LoadMeshFullPath(Mesh_Type Type, const std::string& Name,
		const TCHAR* FullPath, class CScene* Scene = nullptr);
	bool LoadMeshMultibyte(Mesh_Type Type, const std::string& Name, const char* FileName,
		const std::string& PathName = MESH_PATH, class CScene* Scene = nullptr);
	bool LoadMeshFullPathMultibyte(Mesh_Type Type, const std::string& Name,
		const char* FullPath, class CScene* Scene = nullptr);
	bool SetMeshSkeleton(const std::string& Name, class CSkeleton* Skeleton);
	class CMesh* FindMesh(const std::string& Name);
	void ReleaseMesh(const std::string& Name);

private:
	bool CreateSphere(std::vector<Vertex3D>& vecVertex, std::vector<int>& vecIndex, float Radius,
		unsigned int SubDivision);
	void Subdivide(std::vector<Vertex3D>& vecVertices, std::vector<int>& vecIndices);
	float AngleFromXY(float x, float y);
};

