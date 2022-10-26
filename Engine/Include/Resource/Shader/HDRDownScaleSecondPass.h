#pragma once
#include "ComputeShader.h"

class CHDRDownScaleSecondPass :
    public CComputeShader
{
    friend class CShaderManager;

protected:
    CHDRDownScaleSecondPass();
    CHDRDownScaleSecondPass(const CHDRDownScaleSecondPass& shader);
    virtual ~CHDRDownScaleSecondPass();

public:
    virtual bool Init();
};

