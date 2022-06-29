#pragma once

#include "Animation/AnimationSequenceInstance.h"

class CPlayerAnimation :
    public CAnimationSequenceInstance
{
public:
	CPlayerAnimation();
	CPlayerAnimation(const CPlayerAnimation& Anim);
	~CPlayerAnimation();

protected:
	bool        m_Idle;

public:
	bool GetIdleEnable()	const
	{
		return m_Idle;
	}

	void SetIdleEnable(bool Enable)
	{
		m_Idle = Enable;
	}

public:
	virtual bool Init();
	virtual CPlayerAnimation* Clone();

public:
	void AttackEnd();
};

