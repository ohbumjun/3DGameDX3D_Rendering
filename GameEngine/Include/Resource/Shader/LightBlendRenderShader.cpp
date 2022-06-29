
#include "LightBlendRenderShader.h"

CLightBlendRenderShader::CLightBlendRenderShader()
{
	SetTypeID<CLightBlendRenderShader>();
}

CLightBlendRenderShader::~CLightBlendRenderShader()
{
}

bool CLightBlendRenderShader::Init()
{
	if (!LoadVertexShader("LightAccVS", TEXT("Light.fx"), SHADER_PATH))
		return false;

	if (!LoadPixelShader("LightBlendRenderPS", TEXT("Light.fx"), SHADER_PATH))
		return false;

	return true;
}
