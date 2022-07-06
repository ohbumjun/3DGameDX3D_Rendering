#pragma once
#include "SceneComponent.h"
class CLandScape :
    public CSceneComponent
{
	friend class CGameObject;
	friend class CCameraManager;

protected:
	CLandScape();
	CLandScape(const CLandScape& com);
	virtual ~CLandScape();

protected:
	CSharedPtr<class CStaticMesh>	m_Mesh;
	int		m_CountX;
	int		m_CountZ;
	std::vector<Vertex3D>		m_vecVtx;
	std::vector<Vector3>		m_vecPos;
	std::vector<unsigned int>	m_vecIndex;
	std::vector<Vector3>		m_vecFaceNormal;
	std::vector<CSharedPtr<class CMaterial>> m_vecMaterialSlot;
	class CLandScapeConstantBuffer* m_CBuffer;
	Vector3	m_Min;
	Vector3	m_Max;

public:
	int GetCountX() const
	{
		return m_CountX;
	}
	int GetCountZ() const
	{
		return m_CountZ;
	}
	const Vector3& GetMin()	const
	{
		return m_Min;
	}

	const Vector3& GetMax()	const
	{
		return m_Max;
	}

	CMaterial* GetMaterial(int Index = 0)    const
	{
		return m_vecMaterialSlot[Index];
	}

public:
	void CreateLandScape(const std::string& Name,
		int CountX, int CountZ,
		const TCHAR* HeightMap = nullptr,
		const std::string& PathName = TEXTURE_PATH);
	void SetMaterial(class CMaterial* Material, int Index = 0);
	void SetMaterial(const std::string& Name, int Index = 0);
	void AddMaterial(class CMaterial* Material);
	void AddMaterial(const std::string& Name);
	void SetDetailLevel(float Level);
	void SetSplatCount(int Count);
	void CreateNavigationData(class CNavigationMesh* NavMesh);

public:
	float GetHeight(const Vector3& Pos);
	// LandScape 기준 (왼쪽하단에서 오른쪽 상단으로 올라가는 Idx)
	// World Space RaySt, RayDir 정보
	// 1) 결과적으로 Mesh 기준의 Idx 정보 (왼쪽 상단 -> 오른쪽 하단으로 내려오는 순서의 삼각형 Mesh 기준 Idx
	// 2) Ray ~ Triangle 사이의 Dist
	// pair<int MeshIdx, int DistBetw>
	std::optional<float> CheckRayIntersectsTriangle(int LandScapeXIdx, int LandScapeZIdx, const Vector3& RaySt, const Vector3& RayDir);
public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);
	virtual void PrevRender();
	virtual void Render();
	virtual void RenderShadowMap();
	virtual void PostRender();
	virtual CLandScape* Clone();
	virtual void Save(FILE* File);
	virtual void Load(FILE* File);


private:
	void ComputeNormal();
	void ComputeTangent();
};

