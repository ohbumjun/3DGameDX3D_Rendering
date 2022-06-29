
#include "BubbleParticle.h"
#include "Component/ParticleComponent.h"

CBubbleParticle::CBubbleParticle()
{
	SetTypeID<CBubbleParticle>();
}

CBubbleParticle::CBubbleParticle(const CBubbleParticle& obj) :
	CGameObject(obj)
{
	m_Particle = (CParticleComponent*)FindComponent("Particle");
}

CBubbleParticle::~CBubbleParticle()
{
}

bool CBubbleParticle::Init()
{
	m_Particle = CreateComponent<CParticleComponent>("Particle");

	SetRootComponent(m_Particle);

	m_Particle->SetRelativePos(200.f, 50.f, 0.f);

	m_Particle->SetParticle("Bubble");

	SetLifeSpan(5.f);
	//m_Particle->SetSpawnTime(0.1f);

	return true;
}

CBubbleParticle* CBubbleParticle::Clone()
{
	return new CBubbleParticle(*this);
}
