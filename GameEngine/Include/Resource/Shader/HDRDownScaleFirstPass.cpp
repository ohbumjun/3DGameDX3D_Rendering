#include "HDRDownScaleFirstPass.h"

CHDRDownScaleFirstPass::CHDRDownScaleFirstPass()
{
	SetTypeID<CHDRDownScaleFirstPass>();
}


CHDRDownScaleFirstPass::CHDRDownScaleFirstPass(const CHDRDownScaleFirstPass& shader)
{
}

CHDRDownScaleFirstPass::~CHDRDownScaleFirstPass()
{
}

bool CHDRDownScaleFirstPass::Init()
{
	if (!LoadComputeShader("DownScaleFirstPass", TEXT("HDR.fx"), SHADER_PATH))
		return false;

	return true;
}
