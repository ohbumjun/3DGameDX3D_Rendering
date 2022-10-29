#pragma once
#include "GraphicShader.h"

class CToneMappingShader :
    public CGraphicShader
{
    friend class CShaderManager;

protected:
    CToneMappingShader();
    virtual ~CToneMappingShader();

public:
    virtual bool Init();
};

