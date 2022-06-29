
#include "PlayerAnimation.h"

CPlayerAnimation::CPlayerAnimation()
{
	SetTypeID<CPlayerAnimation>();
	m_Idle = true;
}

CPlayerAnimation::CPlayerAnimation(const CPlayerAnimation& Anim) :
	CAnimationSequenceInstance(Anim)
{
}

CPlayerAnimation::~CPlayerAnimation()
{
}

bool CPlayerAnimation::Init()
{
	if (!CAnimationSequenceInstance::Init())
		return false;

	AddAnimation("PlayerIdle", "Idle");
	AddAnimation("PlayerAttack", "Attack", false);
	AddAnimation("PlayerWalk", "Walk");

	SetEndFunction<CPlayerAnimation>("Attack", this, &CPlayerAnimation::AttackEnd);

	return true;
}

CPlayerAnimation* CPlayerAnimation::Clone()
{
	return new CPlayerAnimation(*this);
}

void CPlayerAnimation::AttackEnd()
{
	ChangeAnimation("Idle");
	m_Idle = true;
}
