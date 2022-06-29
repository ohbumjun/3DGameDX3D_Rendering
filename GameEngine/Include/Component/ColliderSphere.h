#pragma once
#include "ColliderComponent.h"
class CColliderSphere :
    public CColliderComponent
{
    friend class CGameObject;

protected:
    CColliderSphere();
    CColliderSphere(const CColliderSphere& com);
    virtual ~CColliderSphere();

protected:
    SphereInfo   m_Info;

public:
    SphereInfo GetInfo() const
    {
        return m_Info;
    }

    void SetInfo(const Vector3& Center, float Radius)
    {
        m_Info.Center = Center;
        m_Info.Radius = Radius;

        SetWorldScale(m_Info.Radius, m_Info.Radius, m_Info.Radius);
    }

public:
    virtual void Start();
    virtual bool Init();
    virtual void Update(float DeltaTime);
    virtual void PostUpdate(float DeltaTime);
    virtual void PrevRender();
    virtual void Render();
    virtual void PostRender();
    virtual CColliderSphere* Clone();
    virtual void Save(FILE* File);
    virtual void Load(FILE* File);
    virtual bool Collision(CColliderComponent* Dest);
    virtual bool CollisionMouse(const Vector2& MousePos);
    virtual bool CollisionRay(const Ray& ray);
};

