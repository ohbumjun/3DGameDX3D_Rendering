#include "HDRDownScaleSecondPass.h"

CHDRDownScaleSecondPass::CHDRDownScaleSecondPass()
{
	SetTypeID<CHDRDownScaleSecondPass>();
}

CHDRDownScaleSecondPass::CHDRDownScaleSecondPass(const CHDRDownScaleSecondPass& shader)
{
}

CHDRDownScaleSecondPass::~CHDRDownScaleSecondPass()
{
}

bool CHDRDownScaleSecondPass::Init()
{
	if (!LoadComputeShader("DownScaleSecondPass", TEXT("HDR.fx"), SHADER_PATH))
		return false;

	return true;
}
