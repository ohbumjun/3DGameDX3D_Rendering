
#include "LightAccShader.h"


CLightAccShader::CLightAccShader()
{
	SetTypeID<CLightAccShader>();
}

CLightAccShader::~CLightAccShader()
{
}

bool CLightAccShader::Init()
{
	if (!LoadVertexShader("LightAccVS", TEXT("Light.fx"), SHADER_PATH))
		return false;

	if (!LoadPixelShader("LightAccPS", TEXT("Light.fx"), SHADER_PATH))
		return false;

	return true;
}
