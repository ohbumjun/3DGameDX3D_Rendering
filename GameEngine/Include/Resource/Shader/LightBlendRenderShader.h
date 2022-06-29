#pragma once
#include "GraphicShader.h"
class CLightBlendRenderShader :
    public CGraphicShader
{
    friend class CShaderManager;

protected:
    CLightBlendRenderShader();
    virtual ~CLightBlendRenderShader();

public:
    virtual bool Init();
};

