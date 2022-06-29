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

public:
    virtual bool Init();
    virtual void Update(float DeltaTime);
    virtual void PostUpdate(float DeltaTime);
    virtual CMonster* Clone();
};

