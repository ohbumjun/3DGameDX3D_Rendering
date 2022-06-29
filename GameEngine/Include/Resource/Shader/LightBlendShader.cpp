
#include "LightBlendShader.h"

CLightBlendShader::CLightBlendShader()
{
	SetTypeID<CLightBlendShader>();
}

CLightBlendShader::~CLightBlendShader()
{
}

bool CLightBlendShader::Init()
{
	if (!LoadVertexShader("LightAccVS", TEXT("Light.fx"), SHADER_PATH))
		return false;

	if (!LoadPixelShader("LightBlendPS", TEXT("Light.fx"), SHADER_PATH))
		return false;

	return true;
}
