#pragma once

#include "GameObject/GameObject.h"
#include "Component/SpriteComponent.h"
#include "Component/ColliderBox2D.h"
#include "Component/CameraComponent.h"
#include "Component/WidgetComponent.h"
#include "Component/NavAgent.h"

class CPlayer2D :
    public CGameObject
{
    friend class CScene;

protected:
    CPlayer2D();
    CPlayer2D(const CPlayer2D& obj);
    virtual ~CPlayer2D();

private:
    CSharedPtr<CSpriteComponent>    m_Sprite;
    CSharedPtr<CSpriteComponent>    m_ChildLeftSprite;
    CSharedPtr<CSpriteComponent>    m_ChildRightSprite;
    CSharedPtr<CSceneComponent>     m_ChildLeftMuzzle;
    CSharedPtr<CSceneComponent>     m_ChildRightMuzzle;
    CSharedPtr<CSceneComponent>     m_ChildRoot;
    CSharedPtr<CSceneComponent>     m_Muzzle;

    CSharedPtr<CSpriteComponent>    m_Child1Sprite;
    CSharedPtr<CSpriteComponent>    m_Child2Sprite;
    CSharedPtr<CSpriteComponent>    m_Child3Sprite;
    CSharedPtr<CSpriteComponent>    m_Child4Sprite;

    CSharedPtr<CColliderBox2D>       m_Body;
    CSharedPtr<CCameraComponent>     m_Camera;

    CSharedPtr<CWidgetComponent>     m_SimpleHUDWidget;

    CSharedPtr<CNavAgent>     m_NavAgent;

    bool        m_SolW;
    float       m_WDistance;
    float       m_Opacity;

public:
    virtual bool Init();
    virtual void Update(float DeltaTime);
    virtual void PostUpdate(float DeltaTime);
    virtual CPlayer2D* Clone();

private:
    void MoveUp(float DeltaTime);
    void MoveDown(float DeltaTime);
    void RotationZInv(float DeltaTime);
    void RotationZ(float DeltaTime);
    void Attack(float DeltaTime);
    void Attack1(float DeltaTime);
    void Skill1(float DeltaTime);
    void MovePointDown(float DeltaTime);
};

