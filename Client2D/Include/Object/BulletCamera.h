#pragma once

#include "GameObject/GameObject.h"
#include "Component/SpriteComponent.h"
#include "Component/ColliderBox2D.h"
#include "Component/CameraComponent.h"

class CBulletCamera :
    public CGameObject
{
    friend class CScene;

protected:
    CBulletCamera();
    CBulletCamera(const CBulletCamera& obj);
    virtual ~CBulletCamera();

private:
    CSharedPtr<class CSpriteComponent>    m_Sprite;
    CSharedPtr<CColliderBox2D>       m_Body;
    CSharedPtr<CCameraComponent>     m_Camera;
    float   m_Distance;

public:
    void SetCollisionProfile(const std::string& Name);

public:
    virtual void Start();
    virtual bool Init();
    virtual void Update(float DeltaTime);
    virtual void PostUpdate(float DeltaTime);
    virtual CBulletCamera* Clone();

private:
    void CollisionCallback(const CollisionResult& result);
};

