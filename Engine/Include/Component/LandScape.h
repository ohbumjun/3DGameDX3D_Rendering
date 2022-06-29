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

