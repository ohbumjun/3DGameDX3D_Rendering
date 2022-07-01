#pragma once
#include "ColliderComponent.h"

class CColliderBox3D :
    public CColliderComponent
{
    friend class CGameObject;

protected:
    CColliderBox3D();
    CColliderBox3D(const CColliderBox3D& com);
    virtual ~CColliderBox3D();

protected:
    Box3DInfo   m_Info;

public:
    Box3DInfo GetInfo() const
    {
        return m_Info;
    }

    void SetInfo(const Vector3& Center, const Vector3& Length)
    {
       // m_Info.Radius = Radius;
       // 
       // SetWorldScale(m_Info.Radius, m_Info.Radius, m_Info.Radius);

       m_Info.Center = Center;
       m_Info.Length = Length;
       
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
    virtual CColliderBox3D* Clone();
    virtual void Save(FILE* File);
    virtual void Load(FILE* File);
    virtual bool Collision(CColliderComponent* Dest);
    virtual bool CollisionMouse(const Vector2& MousePos);
    virtual bool CollisionRay(const Ray& ray);
};

