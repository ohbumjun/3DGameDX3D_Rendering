#pragma once

#include "GameObject\GameObject.h"
#include "Component/StaticMeshComponent.h"

class CWeapon :
    public CGameObject
{
    friend class CScene;

protected:
    CWeapon();
    CWeapon(const CWeapon& obj);
    virtual ~CWeapon();

private:
    CSharedPtr<CStaticMeshComponent>    m_Mesh;

public:
    virtual bool Init();
    virtual void Update(float DeltaTime);
    virtual void PostUpdate(float DeltaTime);
    virtual CWeapon* Clone();
};

