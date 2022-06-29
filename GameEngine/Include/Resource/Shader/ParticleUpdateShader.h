#pragma once

#include "ComputeShader.h"

class CParticleUpdateShader :
    public CComputeShader
{
    friend class CShaderManager;

protected:
    CParticleUpdateShader();
    CParticleUpdateShader(const CParticleUpdateShader& shader);
    virtual ~CParticleUpdateShader();

public:
    virtual bool Init();
    virtual CParticleUpdateShader* Clone();
};

