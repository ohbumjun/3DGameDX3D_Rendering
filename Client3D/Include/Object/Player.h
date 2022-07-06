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
    CSharedPtr<class CColliderBox3D>        m_ColliderBox3D;
    CSharedPtr<class CColliderSphere>        m_ColliderSphere;
    // 원래 Culling 및 Picking 용은 "Sphere"로 판단하는 중이다.
    // 단, 우선적으로 debugging을 위해 파란색으로 SphereInfo로 판단되는 지점을 세팅할 것이다. (범위는 min, max)
    CSharedPtr<class CPickingLayerBox3D>        m_CullingArea3D;
    Vector3     m_Velocity;
    class CPlayerAnimation* m_Animation;
    CSharedPtr<class CWeapon>   m_Weapon;
    CSharedPtr<CNavAgent>     m_NavAgent;
    Vector3 m_DDTPickedPos;
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

private :
    // Picking Layer 들을 보이게 할지 말지 세팅
    void ControlPickingLayerShowEnable(float DeltaTime);
    void ControlColliderLayerShowEnable(float DeltaTime);
};

