#pragma once

#include "GameObject/GameObject.h"

class CBubbleParticle :
    public CGameObject
{
    friend class CScene;

protected:
    CBubbleParticle();
    CBubbleParticle(const CBubbleParticle& obj);
    virtual ~CBubbleParticle();

private:
    CSharedPtr<class CParticleComponent>    m_Particle;

public:
    virtual bool Init();
    virtual CBubbleParticle* Clone();
};

