
#include "ToneMappingShader.h"


CToneMappingShader::CToneMappingShader()
{
	SetTypeID<CToneMappingShader>();
}

CToneMappingShader::~CToneMappingShader()
{
}

bool CToneMappingShader::Init()
{
	if (!LoadVertexShader("ToneMappingVS", TEXT("ToneMapping.fx"), SHADER_PATH))
		return false;

	if (!LoadPixelShader("ToneMappingPS", TEXT("ToneMapping.fx"), SHADER_PATH))
		return false;

	return true;
}
