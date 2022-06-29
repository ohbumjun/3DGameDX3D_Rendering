#pragma once
#include "GraphicShader.h"
class CLightBlendShader :
    public CGraphicShader
{
    friend class CShaderManager;

protected:
    CLightBlendShader();
    virtual ~CLightBlendShader();

public:
    virtual bool Init();
};

