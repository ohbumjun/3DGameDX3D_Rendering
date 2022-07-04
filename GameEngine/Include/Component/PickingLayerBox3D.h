#pragma once
#include "../GameInfo.h"
#include "SceneComponent.h"

class CPickingLayerBox3D : 
    public CSceneComponent
{
    friend class CGameObject;
    friend class CCollision;

    // Debugging 용, 굳이 private 처리할 필요는 없다.
public:
    CPickingLayerBox3D();
    CPickingLayerBox3D(const CPickingLayerBox3D& com);
    virtual ~CPickingLayerBox3D();

protected:
    Box3DInfo   m_Info;
    CSharedPtr<class CMesh> m_Mesh;
    CSharedPtr<class CShader> m_Shader;
    class CColliderConstantBuffer* m_CBuffer;
    Vector3         m_Offset;
    Vector3         m_Min;
    Vector3         m_Max;
public:
    Box3DInfo GetInfo() const
    {
        return m_Info;
    }

    void SetInfo(const Vector3& Center, const Vector3& Length)
    {
        m_Info.Length = Length;
        m_Info.Center = Center;

        SetWorldPos(Center);
        SetWorldScale(m_Info.Length.x * 2.f, m_Info.Length.y * 2.f, m_Info.Length.z * 2.f);
    }

public:
    virtual void Start();
    virtual bool Init();
    virtual void Update(float DeltaTime);
    virtual void PostUpdate(float DeltaTime);
    virtual void PrevRender();
    virtual void Render();
    virtual void PostRender();
};

