
#include "AnimationUpdateShader.h"

CAnimationUpdateShader::CAnimationUpdateShader()
{
	SetTypeID<CAnimationUpdateShader>();
}

CAnimationUpdateShader::CAnimationUpdateShader(const CAnimationUpdateShader& shader) :
	CComputeShader(shader)
{
}

CAnimationUpdateShader::~CAnimationUpdateShader()
{
}

bool CAnimationUpdateShader::Init()
{
	if (!LoadComputeShader("AnimationUpdateCS", TEXT("AnimationUpdate.fx"), SHADER_PATH))
		return false;

	return true;
}

CAnimationUpdateShader* CAnimationUpdateShader::Clone()
{
	return new CAnimationUpdateShader(*this);
}
