#pragma once

#include "GameObject/GameObject.h"
#include "Component/SpriteComponent.h"
#include "Component/PaperBurnComponent.h"
#include "Component/ColliderCircle.h"
#include "Component/WidgetComponent.h"

class CMonster :
    public CGameObject
{
    friend class CScene;

protected:
    CMonster();
    CMonster(const CMonster& obj);
    virtual ~CMonster();

private:
    CSharedPtr<CSpriteComponent>    m_Sprite;
    CSharedPtr<CColliderCircle>       m_Body;
    CSharedPtr<CPaperBurnComponent>   m_PaperBurn;

    CSharedPtr<CWidgetComponent>     m_SimpleHUDWidget;
    class CSimpleHUD* m_SimpleHUD;

    int                 m_HP;

public:
    virtual void Start();
    virtual bool Init();
    virtual void Update(float DeltaTime);
    virtual void PostUpdate(float DeltaTime);
    virtual CMonster* Clone();

public:
    void OnMouseBegin(const CollisionResult& result);
    void OnMouseEnd(const CollisionResult& result);

public:
    void OnCollisionBegin(const CollisionResult& result);
    void OnCollisionEnd(const CollisionResult& result);

private:
    void PaperBurnEnd();
};

