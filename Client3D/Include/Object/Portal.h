#pragma once

#include "GameObject\GameObject.h"
#include "Component/BillboardComponent.h"

class CPortal :
    public CGameObject
{
    friend class CScene;

protected:
    CPortal();
    CPortal(const CPortal& obj);
    virtual ~CPortal();

private:
    CSharedPtr<CBillboardComponent>    m_Billboard;

public:
    virtual bool Init();
    virtual CPortal* Clone();
};

