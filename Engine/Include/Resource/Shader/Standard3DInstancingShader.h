#pragma once
#include "GraphicShader.h"
class CStandard3DInstancingShader :
    public CGraphicShader
{
    friend class CShaderManager;

protected:
    CStandard3DInstancingShader();
    virtual ~CStandard3DInstancingShader();

public:
    virtual bool Init();
};

