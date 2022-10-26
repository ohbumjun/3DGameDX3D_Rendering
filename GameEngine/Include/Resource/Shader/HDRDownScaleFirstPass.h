#pragma once
#include "ComputeShader.h"

class CHDRDownScaleFirstPass :
    public CComputeShader
{
    friend class CShaderManager;

protected:
    CHDRDownScaleFirstPass();
    CHDRDownScaleFirstPass(const CHDRDownScaleFirstPass& shader);
    virtual ~CHDRDownScaleFirstPass();

public:
    virtual bool Init();
};

