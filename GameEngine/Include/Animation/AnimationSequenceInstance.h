#pragma once

#include "AnimationSequenceData.h"

class CAnimationSequenceInstance
{
public:
	CAnimationSequenceInstance();
	CAnimationSequenceInstance(const CAnimationSequenceInstance& Anim);
	virtual ~CAnimationSequenceInstance();

protected:
	size_t		m_TypeID;
	class CAnimationMeshComponent* m_Owner;
	class CScene* m_Scene;
	std::unordered_map<std::string, CAnimationSequenceData*>	m_mapAnimation;
	CAnimationSequenceData* m_CurrentAnimation;
	CAnimationSequenceData* m_ChangeAnimation;
	bool		m_PlayAnimation;

	CSharedPtr<class CComputeShader>	m_AnimationUpdateShader;
	class CAnimationUpdateConstantBuffer* m_AnimationUpdateCBuffer;
	class CStructuredBuffer* m_OutputBuffer;	// 애니메이션 결과 저장용 버퍼
	class CStructuredBuffer* m_BoneBuffer;
	class CStructuredBuffer* m_InstancingBoneBuffer;
	std::vector<Matrix>	m_vecBoneMatrix;
	CSharedPtr<class CSkeleton>	m_Skeleton;

	Vector3	m_BlendScale;
	Vector3	m_BlendPos;
	Vector4	m_BlendRot;
	float	m_GlobalTime;
	float	m_SequenceProgress;
	float	m_ChangeTimeAcc;
	float	m_ChangeTime;


public:
	void SetInstancingBoneBuffer(class CStructuredBuffer* Buffer)
	{
		m_InstancingBoneBuffer = Buffer;
	}

	size_t GetTypeID()	const
	{
		return m_TypeID;
	}

	template <typename T>
	void SetTypeID()
	{
		m_TypeID = typeid(T).hash_code();
	}

	void SetScene(class CScene* Scene)
	{
		m_Scene = Scene;
	}

	void SetOwner(class CAnimationMeshComponent* Owner)
	{
		m_Owner = Owner;
	}

	int GetAnimationCount()	const
	{
		return (int)m_mapAnimation.size();
	}

	void Play()
	{
		m_PlayAnimation = true;
	}

	void Stop()
	{
		m_PlayAnimation = false;
	}

	bool IsPlay()	const
	{
		return m_PlayAnimation;
	}

	CAnimationSequenceData* GetCurrentAnimation()	const
	{
		return m_CurrentAnimation;
	}

	void SetSkeleton(class CSkeleton* Skeleton);

public:
	void AddAnimation(const std::string& SequenceName, const std::string& Name, bool Loop = true, float PlayTime = 1.f,
		float PlayScale = 1.f);
	void AddAnimation(const TCHAR* FileName, const std::string& PathName, const std::string& Name, bool Loop = true, float PlayTime = 1.f,
		float PlayScale = 1.f);
	void SetPlayTime(const std::string& Name, float PlayTime);
	void SetPlayScale(const std::string& Name, float PlayScale);
	void SetLoop(const std::string& Name, bool Loop);
	void SetCurrentAnimation(const std::string& Name);
	void ChangeAnimation(const std::string& Name);
	bool CheckCurrentAnimation(const std::string& Name);

public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float DeltaTime);
	void SetShader();
	void ResetShader();
	virtual CAnimationSequenceInstance* Clone();
	virtual void Save(FILE* File);
	virtual void Load(FILE* File);

private:
	CAnimationSequenceData* FindAnimation(const std::string& Name);

public:
	template <typename T>
	void SetEndFunction(const std::string& Name, T* Obj, void (T::* Func)())
	{
		CAnimationSequenceData* Data = FindAnimation(Name);

		if (!Data)
			return;

		Data->SetEndFunction<T>(Obj, Func);
	}

	template <typename T>
	void AddNotify(const std::string& Name, const std::string& NotifyName, int Frame,
		T* Obj, void (T::* Func)())
	{
		CAnimationSequenceData* Data = FindAnimation(Name);

		if (!Data)
			return;

		Data->AddNotify<T>(NotifyName, Frame, Obj, Func);
	}
};

