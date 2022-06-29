#pragma once

#include "AnimationSequence.h"
#include "Skeleton.h"

class CAnimationManager3D
{
	friend class CResourceManager;

private:
	CAnimationManager3D();
	~CAnimationManager3D();

private:
	std::unordered_map<std::string, CSharedPtr<CAnimationSequence>>	m_mapSequence;
	std::unordered_map<std::string, CSharedPtr<CSkeleton>>	m_mapSkeleton;

public:
	bool Init();
	bool LoadAnimationSequence(const std::string& Name, bool Loop,
		struct _tagFbxAnimationClip* Clip, class CScene* Scene = nullptr);
	bool LoadAnimationSequence(const std::string& Name, bool Loop,
		int StartFrame, int EndFrame, float PlayTime,
		const std::vector<BoneKeyFrame*>& vecFrame, class CScene* Scene = nullptr);
	bool LoadAnimationSequence(bool Loop, const std::string& Name, const TCHAR* FileName,
		const std::string& PathName = ANIMATION_PATH, class CScene* Scene = nullptr);
	bool LoadAnimationSequenceFullPath(bool Loop, const std::string& Name, const TCHAR* FullPath, class CScene* Scene = nullptr);
	bool LoadAnimationSequenceMultibyte(bool Loop, const std::string& Name, const char* FileName,
		const std::string& PathName = ANIMATION_PATH, class CScene* Scene = nullptr);
	bool LoadAnimationSequenceFullPathMultibyte(bool Loop, const std::string& Name, const char* FullPath, class CScene* Scene = nullptr);
	CAnimationSequence* FindAnimationSequence(const std::string& Name);
	void ReleaseSequence(const std::string& Name);


	bool LoadSkeleton(const std::string& Name, const TCHAR* FileName,
		const std::string& PathName = ANIMATION_PATH,
		class CScene* Scene = nullptr);
	bool LoadSkeletonFullPath(const std::string& Name, const TCHAR* FullPath,
		class CScene* Scene = nullptr);
	bool LoadSkeletonMultibyte(const std::string& Name, const char* FileName,
		const std::string& PathName = ANIMATION_PATH,
		class CScene* Scene = nullptr);
	bool LoadSkeletonFullPathMultibyte(const std::string& Name, const char* FullPath,
		class CScene* Scene = nullptr);
	bool AddSocket(const std::string& SkeletonName, const std::string& BoneName, const std::string& SocketName,
		const Vector3& Offset = Vector3::Zero, const Vector3& OffsetRot = Vector3::Zero);
	CSkeleton* FindSkeleton(const std::string& Name);
	void ReleaseSkeleton(const std::string& Name);
};

