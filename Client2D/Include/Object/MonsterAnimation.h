#pragma once

#include "Animation/AnimationSequence2DInstance.h"

class CMonsterAnimation :
    public CAnimationSequence2DInstance
{
public:
	CMonsterAnimation();
	CMonsterAnimation(const CMonsterAnimation& Anim);
	~CMonsterAnimation();

public:
	virtual bool Init();
	virtual CMonsterAnimation* Clone();
};

