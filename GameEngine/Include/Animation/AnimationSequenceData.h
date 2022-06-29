#pragma once

#include "../GameInfo.h"

struct AnimationNotify
{
	std::string	Name;
	int		Frame;
	bool	Call;
	std::function<void()>	Function;

	AnimationNotify() :
		Frame(0),
		Call(false)
	{
	}
};

class CAnimationSequenceData
{
	friend class CAnimationSequenceInstance;

private:
	CAnimationSequenceData();
	~CAnimationSequenceData();

private:
	std::string	m_Name;
	std::string	m_SequenceName;
	CSharedPtr<class CAnimationSequence>	m_Sequence;
	float	m_Time;			// 애니메이션 동작 시간
	float	m_FrameTime;	// 1프레임당 시간
	float	m_PlayTime;
	float	m_PlayScale;		// 재생 비율
	bool	m_Loop;
	std::function<void()>	m_EndFunction;
	std::vector<AnimationNotify*>	m_vecNotify;

public:
	void Save(FILE* File);
	void Load(FILE* File);

public:
	const std::string& GetName()	const
	{
		return m_Name;
	}

	float GetAnimationTime()	const
	{
		return m_Time;
	}

	class CAnimationSequence* GetAnimationSequence()	const
	{
		return m_Sequence;
	}

public:
	template <typename T>
	void SetEndFunction(T* Obj, void (T::* Func)())
	{
		m_EndFunction = std::bind(Func, Obj);
	}

	template <typename T>
	void AddNotify(const std::string& Name, int Frame, T* Obj, void (T::* Func)())
	{
		AnimationNotify* Notify = new AnimationNotify;

		Notify->Name = Name;
		Notify->Frame = Frame;
		Notify->Function = std::bind(Func, Obj);

		m_vecNotify.push_back(Notify);
	}
};

