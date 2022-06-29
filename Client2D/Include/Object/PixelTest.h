#pragma once

#include "GameObject/GameObject.h"

class CPixelTest :
    public CGameObject
{
    friend class CScene;

protected:
    CPixelTest();
    CPixelTest(const CPixelTest& obj);
    virtual ~CPixelTest();

private:
    CSharedPtr<class CColliderPixel>    m_Body;

public:
    virtual bool Init();
    virtual CPixelTest* Clone();
};

