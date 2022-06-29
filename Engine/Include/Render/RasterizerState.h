#pragma once
#include "RenderState.h"
class CRasterizerState :
    public CRenderState
{
    friend class CRenderStateManager;

protected:
    CRasterizerState();
    virtual ~CRasterizerState();

protected:

public:

public:
    bool CreateState(D3D11_FILL_MODE FillMode,
        D3D11_CULL_MODE CullMode = D3D11_CULL_BACK,
        BOOL FrontCounterClockwise = FALSE,
        INT DepthBias = 0, FLOAT DepthBiasClamp = 0.f,
        FLOAT SlopeScaledDepthBias = 0.f, BOOL DepthClipEnable = TRUE,
        BOOL ScissorEnable = FALSE, BOOL MultisampleEnable = FALSE,
        BOOL AntialiasedLineEnable = FALSE);

public:
    virtual void SetState();
    virtual void ResetState();
};

