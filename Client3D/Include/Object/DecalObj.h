#pragma once

#include "GameObject\GameObject.h"
#include "Component/DecalComponent.h"

class CDecalObj :
    public CGameObject
{
    friend class CScene;

protected:
    CDecalObj();
    CDecalObj(const CDecalObj& obj);
    virtual ~CDecalObj();

private:
    CSharedPtr<CDecalComponent>    m_Decal;

public:
    virtual bool Init();
    virtual void Update(float DeltaTime);
    virtual void PostUpdate(float DeltaTime);
    virtual CDecalObj* Clone();
};

