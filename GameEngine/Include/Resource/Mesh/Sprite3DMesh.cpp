
#include "Sprite3DMesh.h"

CSprite3DMesh::CSprite3DMesh()
{
	SetTypeID<CSprite3DMesh>();
	m_MeshType = Mesh_Type::Sprite;
}

CSprite3DMesh::~CSprite3DMesh()
{
}

bool CSprite3DMesh::Init()
{
	MeshContainer* Container = new MeshContainer;

	Container->VB.Size = sizeof(VertexUV3D);
	Container->VB.Count = 5;
	Container->Primitive = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	VertexUV3D	FlatMesh3DVtx[5] =
	{
		VertexUV3D(Vector3(0.f, 1.f, 0.f), Vector3(0.f, 0.f, 0.f)),
		VertexUV3D(Vector3(1.f, 1.f, 0.f), Vector3(1.f, 0.f, 0.f)),
		VertexUV3D(Vector3(0.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f)),
		VertexUV3D(Vector3(1.f, 0.f, 0.f), Vector3(1.f, 1.f, 0.f)),
		VertexUV3D(Vector3(0.f, 1.f, 0.f), Vector3(0.f, 0.f, 0.f))
	};

	if (!CreateBuffer(Buffer_Type::Vertex, FlatMesh3DVtx, sizeof(VertexUV3D),
		5, D3D11_USAGE_IMMUTABLE, &Container->VB.Buffer))
		return false;

	Container->vecIB.resize(1);

	Container->vecIB[0].Size = 2;
	Container->vecIB[0].Count = 6;
	Container->vecIB[0].Fmt = DXGI_FORMAT_R16_UINT;

	unsigned short Idx[6] = { 0, 1, 3, 0, 3, 2 };

	if (!CreateBuffer(Buffer_Type::Index, Idx, sizeof(unsigned short),
		6, D3D11_USAGE_IMMUTABLE, &Container->vecIB[0].Buffer))
		return false;

	m_vecContainer.push_back(Container);

	return true;
}
