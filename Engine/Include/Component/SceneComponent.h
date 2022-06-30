#pragma once

#include "Component.h"
#include "Transform.h"
#include "../Resource/Mesh/Mesh.h"
#include "../Resource/Material/Material.h"

struct InstancingCheckCount
{
	std::string		LayerName;
	CMesh* Mesh;
	std::list<class CSceneComponent*>	InstancingList;

	InstancingCheckCount() :
		Mesh(nullptr)
	{
	}
};

class CSceneComponent :
    public CComponent
{
    friend class CGameObject;

protected:
    CSceneComponent();
    CSceneComponent(const CSceneComponent& com);
    virtual ~CSceneComponent();

protected:
    CTransform* m_Transform;
    CSceneComponent* m_Parent;
    std::vector<CSharedPtr<CSceneComponent>>    m_vecChild;
    bool    m_Render;
	std::string		m_LayerName;
	class CSkeletonSocket* m_Socket;
	SphereInfo	m_SphereInfo;
	bool	m_Culling;
	bool	m_ReceiveDecal;
	bool	m_Instancing;
	static std::list<InstancingCheckCount*>	m_InstancingCheckList;

public:
	void SetInstancingInfo(Instancing3DInfo* Info);
	void SetInstancingShadowInfo(Instancing3DInfo* Info);

	static const std::list<InstancingCheckCount*>* GetInstancingCheckList()
	{
		return &m_InstancingCheckList;
	}

	static void DestroyInstancingCheckList()
	{
		auto	iter = m_InstancingCheckList.begin();
		auto	iterEnd = m_InstancingCheckList.end();

		for (; iter != iterEnd; ++iter)
		{
			SAFE_DELETE((*iter));
		}
	}

public:
	bool GetInstancing()	const
	{
		return m_Instancing;
	}

	bool GetRender()	const
	{
		return m_Render;
	}

	bool GetCulling()	const
	{
		return m_Culling;
	}

	SphereInfo GetSphereInfo()	const
	{
		SphereInfo	Info;

		//Info.Center = m_SphereInfo.Center * GetWorldScale() + GetWorldPos();

		// Mesh 입장에서는 자신에게 부착된 Bone 의 변환 행렬 정보까지 모두 반영되어 만들어진
		// World 행렬 정보를 이용해서, 정보를 얻어야 한다.
		Info.Center = m_SphereInfo.Center.TransformCoord(GetWorldMatrix());
		Info.Radius = m_SphereInfo.Radius;

		// (CheckCollision 에서도 똑같이 세팅)
		//  ex. Player 의 경우, 어떠한 처리도 하지 않으면, 발밑으로 World Pos가 잡히고
		// 이로 인해, Center도 발밑으로 잡힌다.
		// 하지만, 제대로 Culling을 세팅하려면, Center 가 몸 중앙에 와야 한다.
		// 따라서 SphereInfo에 별도의 Offset 을 줘서 이를 조정해줘야 한다.
		// 정확히는 World 공간상..y 값 ?
		// 실질적인 Offset 은 MeshSize * Worldscale 값에 의존한다.
		// WorldScale 값을 실시간 변하지만, MeshSize는 바뀌지 않는다.
		// 따라서 MeshSize 를 세팅해두고, 여기에 실시간 바뀌는 World Scale 정보를 세팅하는 방법이 있다.
		// 혹은, 해당 MeshSize 는, Root Component 의 Transform 의 MeshSize 라는 변수에 들어있으므로
		// 해당 변수 내용을 이용해도 된다.
		Info.Center.y += 0.5f * (m_Transform->GetMeshSize().y * GetRelativeScale().y);

		return Info;
	}

	SphereInfo GetSphereInfoViewSpace()	const;

	const SphereInfo& GetSphereOriginInfo()	const
	{
		return m_SphereInfo;
	}

	CTransform* GetTransform()	const
	{
		return m_Transform;
	}

	std::string GetLayerName()	const
	{
		return m_LayerName;
	}

    bool IsRender() const
    {
        return m_Render;
    }

public:
	void SetInstancing(bool Instancing)
	{
		m_Instancing = Instancing;
	}

	void SetReceiveDecal(bool Decal)
	{
		m_ReceiveDecal = false;
	}

	void SetLayerName(const std::string& Name)
	{
		m_LayerName = Name;
	}

public:
	void SetSceneComponent(class CGameObject* Object);
	void GetAllSceneComponentsName(std::vector<FindComponentName>& vecNames);
	virtual void Destroy();

public:
    virtual void SetScene(class CScene* Scene);
    virtual void SetGameObject(class CGameObject* Object);

public:
	virtual void AddChild(CSceneComponent* Child, const std::string& SocketName = "");
	virtual void AddChild(class CGameObject* Child, const std::string& SocketName = "");
    bool DeleteChild(CSceneComponent* Child);
    bool DeleteChild(const std::string& Name);
    CSceneComponent* FindComponent(const std::string& Name);

public:
    virtual void Start();
    virtual bool Init();
    virtual void Update(float DeltaTime);
    virtual void PostUpdate(float DeltaTime);
	virtual void CheckCollision();
    virtual void PrevRender();
    virtual void Render();
	virtual void RenderShadowMap();
	virtual void RenderDebug();
    virtual void PostRender();
    virtual CSceneComponent* Clone();
	virtual void Save(FILE* File);
	virtual void Load(FILE* File);





public:	// =============== 저주받은 Transform 영역 ===============
	void SetInheritScale(bool Inherit)
	{
		m_Transform->SetInheritScale(Inherit);
	}

	void SetInheritRotX(bool Inherit)
	{
		m_Transform->SetInheritRotX(Inherit);
	}

	void SetInheritRotY(bool Inherit)
	{
		m_Transform->SetInheritRotY(Inherit);
	}

	void SetInheritRotZ(bool Inherit)
	{
		m_Transform->SetInheritRotZ(Inherit);
	}

	void SetInheritParentRotationPosX(bool Inherit)
	{
		m_Transform->SetInheritParentRotationPosX(Inherit);
	}

	void SetInheritParentRotationPosY(bool Inherit)
	{
		m_Transform->SetInheritParentRotationPosY(Inherit);
	}

	void SetInheritParentRotationPosZ(bool Inherit)
	{
		m_Transform->SetInheritParentRotationPosZ(Inherit);
	}

	void InheritScale(bool Current)
	{
		m_Transform->InheritScale(Current);
	}

	void InheritRotation(bool Current)
	{
		m_Transform->InheritRotation(Current);
	}

	void InheritParentRotationPos(bool Current)
	{
		m_Transform->InheritParentRotationPos(Current);
	}

	void InheritWorldScale(bool Current)
	{
		m_Transform->InheritWorldScale(Current);
	}

	void InheritWorldRotation(bool Current)
	{
		m_Transform->InheritWorldRotation(Current);
	}

public:
	Vector3 GetRelativeScale()	const
	{
		return m_Transform->GetRelativeScale();
	}

	Vector3 GetRelativeRot()	const
	{
		return m_Transform->GetRelativeRot();
	}

	Vector3 GetRelativePos()	const
	{
		return m_Transform->GetRelativePos();
	}

	Vector3 GetRelativeAxis(AXIS Axis)
	{
		return m_Transform->GetRelativeAxis(Axis);
	}

public:
	void SetRelativeScale(const Vector3& Scale)
	{
		m_Transform->SetRelativeScale(Scale);
	}

	void SetRelativeScale(float x, float y, float z)
	{
		m_Transform->SetRelativeScale(x, y, z);
	}

	void SetRelativeRotation(const Vector3& Rot)
	{
		m_Transform->SetRelativeRotation(Rot);
	}

	void SetRelativeRotation(float x, float y, float z)
	{
		m_Transform->SetRelativeRotation(x, y, z);
	}

	void SetRelativeRotationX(float x)
	{
		m_Transform->SetRelativeRotationX(x);
	}

	void SetRelativeRotationY(float y)
	{
		m_Transform->SetRelativeRotationY(y);
	}

	void SetRelativeRotationZ(float z)
	{
		m_Transform->SetRelativeRotationZ(z);
	}

	void SetRelativePos(const Vector3& Pos)
	{
		m_Transform->SetRelativePos(Pos);
	}

	void SetRelativePos(float x, float y, float z)
	{
		m_Transform->SetRelativePos(x, y, z);
	}

	void AddRelativeScale(const Vector3& Scale)
	{
		m_Transform->AddRelativeScale(Scale);
	}

	void AddRelativeScale(float x, float y, float z)
	{
		m_Transform->AddRelativeScale(x, y, z);
	}

	void AddRelativeRotation(const Vector3& Rot)
	{
		m_Transform->AddRelativeRotation(Rot);
	}

	void AddRelativeRotation(float x, float y, float z)
	{
		m_Transform->AddRelativeRotation(x, y, z);
	}

	void AddRelativeRotationX(float x)
	{
		m_Transform->AddRelativeRotationX(x);
	}

	void AddRelativeRotationY(float y)
	{
		m_Transform->AddRelativeRotationY(y);
	}

	void AddRelativeRotationZ(float z)
	{
		m_Transform->AddRelativeRotationZ(z);
	}

	void AddRelativePos(const Vector3& Pos)
	{
		m_Transform->AddRelativePos(Pos);
	}

	void AddRelativePos(float x, float y, float z)
	{
		m_Transform->AddRelativePos(x, y, z);
	}

public:
	Vector3 GetWorldScale()	const
	{
		return m_Transform->GetWorldScale();
	}

	Vector3 GetWorldRot()	const
	{
		return m_Transform->GetWorldRot();
	}

	Vector3 GetWorldPos()	const
	{
		return m_Transform->GetWorldPos();
	}

	Vector3 GetPivot()	const
	{
		return m_Transform->GetPivot();
	}

	Vector3 GetMeshSize()	const
	{
		return m_Transform->GetMeshSize();
	}

	const Matrix& GetWorldMatrix()	const
	{
		return m_Transform->GetWorldMatrix();
	}

	Vector3 GetWorldAxis(AXIS Axis)
	{
		return m_Transform->GetWorldAxis(Axis);
	}

public:
	void SetPivot(const Vector3& Pivot)
	{
		m_Transform->SetPivot(Pivot);
	}

	void SetPivot(float x, float y, float z)
	{
		m_Transform->SetPivot(x, y, z);
	}

	void SetMeshSize(const Vector3& Size)
	{
		m_Transform->SetMeshSize(Size);
	}

	void SetMeshSize(float x, float y, float z)
	{
		m_Transform->SetMeshSize(x, y, z);
	}

public:
	void SetWorldScale(const Vector3& Scale)
	{
		m_Transform->SetWorldScale(Scale);
	}

	void SetWorldScale(float x, float y, float z)
	{
		m_Transform->SetWorldScale(x, y, z);
	}

	void SetWorldRotation(const Vector3& Rot)
	{
		m_Transform->SetWorldRotation(Rot);
	}

	void SetWorldRotation(float x, float y, float z)
	{
		m_Transform->SetWorldRotation(x, y, z);
	}

	void SetWorldRotationX(float x)
	{
		m_Transform->SetWorldRotationX(x);
	}

	void SetWorldRotationY(float y)
	{
		m_Transform->SetWorldRotationY(y);
	}

	void SetWorldRotationZ(float z)
	{
		m_Transform->SetWorldRotationZ(z);
	}

	void SetWorldPos(const Vector3& Pos)
	{
		m_Transform->SetWorldPos(Pos);
	}

	void SetWorldPos(float x, float y, float z)
	{
		m_Transform->SetWorldPos(x, y, z);
	}

	void AddWorldScale(const Vector3& Scale)
	{
		m_Transform->AddWorldScale(Scale);
	}

	void AddWorldScale(float x, float y, float z)
	{
		m_Transform->AddWorldScale(x, y, z);
	}

	void AddWorldRotation(const Vector3& Rot)
	{
		m_Transform->AddWorldRotation(Rot);
	}

	void AddWorldRotation(float x, float y, float z)
	{
		m_Transform->AddWorldRotation(x, y, z);
	}

	void AddWorldRotationX(float x)
	{
		m_Transform->AddWorldRotationX(x);
	}

	void AddWorldRotationY(float y)
	{
		m_Transform->AddWorldRotationY(y);
	}

	void AddWorldRotationZ(float z)
	{
		m_Transform->AddWorldRotationZ(z);
	}

	void AddWorldPos(const Vector3& Pos)
	{
		m_Transform->AddWorldPos(Pos);
	}

	void AddWorldPos(float x, float y, float z)
	{
		m_Transform->AddWorldPos(x, y, z);
	}
};

