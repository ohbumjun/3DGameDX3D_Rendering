#pragma once
#include "GraphicShader.h"
class CShadowMapShader :
    public CGraphicShader
{
    friend class CShaderManager;

protected:
    CShadowMapShader();
    virtual ~CShadowMapShader();

public:
    virtual bool Init();
};

