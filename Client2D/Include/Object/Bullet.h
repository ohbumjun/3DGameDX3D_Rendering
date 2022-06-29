#pragma once

#include "GameObject/GameObject.h"
#include "Component/ColliderCircle.h"

class CBullet :
    public CGameObject
{
    friend class CScene;

protected:
    CBullet();
    CBullet(const CBullet& obj);
    virtual ~CBullet();

private:
    CSharedPtr<class CSpriteComponent>    m_Sprite;
    CSharedPtr<CColliderCircle>       m_Body;
    float   m_Distance;

public:
    void SetCollisionProfile(const std::string& Name);

public:
    virtual bool Init();
    virtual void Update(float DeltaTime);
    virtual void PostUpdate(float DeltaTime);
    virtual CBullet* Clone();

public:
    void OnCollisionBegin(const CollisionResult& result);
    void OnCollisionEnd(const CollisionResult& result);
};

