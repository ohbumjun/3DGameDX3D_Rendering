#pragma once
#include "GraphicShader.h"
class CShadowMapInstancingShader :
    public CGraphicShader
{
    friend class CShaderManager;

protected:
    CShadowMapInstancingShader();
    virtual ~CShadowMapInstancingShader();

public:
    virtual bool Init();
};

