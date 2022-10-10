#pragma once
#include "GraphicShader.h"

class CWaterShader :
    public CGraphicShader
{
    friend class CShaderManager;

protected:
    CWaterShader();
    virtual ~CWaterShader();

public:
    virtual bool Init();
};

