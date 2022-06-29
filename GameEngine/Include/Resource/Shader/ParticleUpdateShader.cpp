
#include "ParticleUpdateShader.h"

CParticleUpdateShader::CParticleUpdateShader()
{
	SetTypeID<CParticleUpdateShader>();
}

CParticleUpdateShader::CParticleUpdateShader(const CParticleUpdateShader& shader)	:
	CComputeShader(shader)
{
}

CParticleUpdateShader::~CParticleUpdateShader()
{
}

bool CParticleUpdateShader::Init()
{
	if (!LoadComputeShader("ParticleUpdate", TEXT("Particle.fx"), SHADER_PATH))
		return false;

	return true;
}

CParticleUpdateShader* CParticleUpdateShader::Clone()
{
	return new CParticleUpdateShader(*this);
}
