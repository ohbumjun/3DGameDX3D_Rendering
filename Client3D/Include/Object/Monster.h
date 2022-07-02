#pragma once

#include "GameObject\GameObject.h"
#include "Component/AnimationMeshComponent.h"

class CMonster :
    public CGameObject
{
    friend class CScene;

protected:
    CMonster();
    CMonster(const CMonster& obj);
    virtual ~CMonster();

private:
    CSharedPtr<CAnimationMeshComponent>    m_Mesh;
    class CMonsterAnimation* m_Animation;
    CSharedPtr<class CColliderBox3D>        m_ColliderBox3D;
    // CSharedPtr<class CColliderSphere>        m_ColliderSphere;

public:
    virtual bool Init();
    virtual void Update(float DeltaTime);
    virtual void PostUpdate(float DeltaTime);
    virtual CMonster* Clone();
};

