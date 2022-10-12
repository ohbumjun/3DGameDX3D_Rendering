#pragma once

#include "Component/SceneComponent.h"
#include "Resource/Mesh/SpriteMesh.h"
#include "Resource/Material/Material.h"

class CWaterComponent :
    public CSceneComponent
{
	friend class CGameObject;
	friend class CCameraManager;

protected:
	CWaterComponent();
	CWaterComponent(const CWaterComponent& com);
	virtual ~CWaterComponent();

protected:
	// CSharedPtr<CSpriteMesh> m_Mesh;
	CSharedPtr<class CStaticMesh>	m_Mesh;
	// CSharedPtr<CSprite3DMesh>	m_Mesh;

	CSharedPtr<CMaterial> m_Material; // Water Material
	CSharedPtr<CMaterial> m_SkyMaterial; // Water Material
	class CWaterConstantBuffer* m_CBuffer;
	Vector3	m_Min;
	Vector3	m_Max;
	unsigned		m_CountX;
	unsigned		m_CountZ;
	std::vector<Vertex3D>		m_vecVtx;
	std::vector<Vector3>		m_vecPos;
	std::vector<unsigned int>	m_vecIndex;
	std::vector<Vector3>		m_vecFaceNormal;
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
		return m_Material;
	}

public:
	void CreateWaterMesh(const std::string& Name,
		int CountX, int CountZ);
	void SetMesh(const std::string& Name);
	void SetMaterial(class CMaterial* Material, int Index = 0);
	void SetMaterial(const std::string& Name, int Index = 0);
	void SetWaveHeight(float WaveHeight);
	void SetWaveSpeed(float WaveSpeed);
	void SetWaveFrequency(float WaveFrequencey);
public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);
	virtual void PrevRender();
	virtual void Render();
	virtual void PostRender();
	virtual CWaterComponent* Clone();
	virtual void Save(FILE* File);
	virtual void Load(FILE* File);
private:
	void ComputeNormal();
	void ComputeTangent();

};

