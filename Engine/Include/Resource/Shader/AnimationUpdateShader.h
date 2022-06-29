#pragma once
#include "ComputeShader.h"
class CAnimationUpdateShader :
    public CComputeShader
{
    friend class CShaderManager;

protected:
    CAnimationUpdateShader();
    CAnimationUpdateShader(const CAnimationUpdateShader& shader);
    virtual ~CAnimationUpdateShader();

public:
    virtual bool Init();
    virtual CAnimationUpdateShader* Clone();
};

