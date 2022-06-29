#pragma once

#include "GameObject\GameObject.h"
#include "Component/StaticMeshComponent.h"

class CBonObj :
    public CGameObject
{
    friend class CScene;

protected:
    CBonObj();
    CBonObj(const CBonObj& obj);
    virtual ~CBonObj();

private:
    CSharedPtr<CStaticMeshComponent>    m_Mesh;

public:
    virtual bool Init();
    virtual void Update(float DeltaTime);
    virtual void PostUpdate(float DeltaTime);
    virtual CBonObj* Clone();
};

