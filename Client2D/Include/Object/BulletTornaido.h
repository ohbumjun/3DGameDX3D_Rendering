#pragma once

#include "GameObject/GameObject.h"
#include "Component/ColliderCircle.h"

class CBulletTornaido :
    public CGameObject
{
    friend class CScene;

protected:
    CBulletTornaido();
    CBulletTornaido(const CBulletTornaido& obj);
    virtual ~CBulletTornaido();

private:
    CSharedPtr<class CSceneComponent>    m_Root;
    CSharedPtr<class CSceneComponent>    m_Root1;
    CSharedPtr<class CSpriteComponent>    m_Sprite;
    CSharedPtr<CColliderCircle>       m_Body;

public:
    void SetCollisionProfile(const std::string& Name);

public:
    virtual bool Init();
    virtual void Update(float DeltaTime);
    virtual void PostUpdate(float DeltaTime);
    virtual CBulletTornaido* Clone();

public:
    void OnCollisionBegin(const CollisionResult& result);
    void OnCollisionEnd(const CollisionResult& result);
};

