#pragma once

#include "GameObject/GameObject.h"
#include "Component/AnimationMeshComponent.h"
#include "Component/CameraComponent.h"
#include "Component/Arm.h"
#include "Component/NavAgent.h"

class CPlayer :
    public CGameObject
{
    friend class CScene;

protected:
    CPlayer();
    CPlayer(const CPlayer& obj);
    virtual ~CPlayer();

private:
    CSharedPtr<CAnimationMeshComponent>    m_Mesh;
    CSharedPtr<CArm>                    m_Arm;
    CSharedPtr<CCameraComponent>        m_Camera;
    Vector3     m_Velocity;
    class CPlayerAnimation* m_Animation;
    CSharedPtr<class CWeapon>   m_Weapon;
    CSharedPtr<CNavAgent>     m_NavAgent;

public:
    virtual bool Init();
    virtual void Update(float DeltaTime);
    virtual void PostUpdate(float DeltaTime);
    virtual CPlayer* Clone();

private:
    void MoveFront(float DeltaTime);
    void MoveBack(float DeltaTime);
    void RotationYInv(float DeltaTime);
    void RotationY(float DeltaTime);
    void Attack(float DeltaTime);
    void MovePoint(float DeltaTime);
};

