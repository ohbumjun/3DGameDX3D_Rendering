#pragma once

#include "Animation\AnimationSequenceInstance.h"

class CMonsterAnimation :
    public CAnimationSequenceInstance
{
public:
	CMonsterAnimation();
	CMonsterAnimation(const CMonsterAnimation& Anim);
	~CMonsterAnimation();

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
	virtual CMonsterAnimation* Clone();
};

