
#include "AnimationManager3D.h"
#include "../../PathManager.h"

CAnimationManager3D::CAnimationManager3D()
{
}

CAnimationManager3D::~CAnimationManager3D()
{
}

bool CAnimationManager3D::Init()
{
	return true;
}

bool CAnimationManager3D::LoadAnimationSequence(const std::string& Name, bool Loop, 
	_tagFbxAnimationClip* Clip, class CScene* Scene)
{
	if (FindAnimationSequence(Name))
	{
		return true;
	}

	CAnimationSequence* Sequence = new CAnimationSequence;

	Sequence->m_Scene = Scene;

	if (!Sequence->CreateSequence(Loop, Clip))
	{
		SAFE_DELETE(Sequence);
		return false;
	}

	Sequence->SetName(Name);

	m_mapSequence.insert(std::make_pair(Name, Sequence));

	return true;
}

bool CAnimationManager3D::LoadAnimationSequence(const std::string& Name, bool Loop, 
	int StartFrame, int EndFrame, float PlayTime,
	const std::vector<BoneKeyFrame*>& vecFrame, class CScene* Scene)
{
	if (FindAnimationSequence(Name))
	{
		return true;
	}

	CAnimationSequence* Sequence = new CAnimationSequence;

	Sequence->m_Scene = Scene;

	if (!Sequence->CreateSequence(Name, Loop, StartFrame, EndFrame, PlayTime, vecFrame))
	{
		SAFE_DELETE(Sequence);
		return false;
	}

	Sequence->SetName(Name);

	m_mapSequence.insert(std::make_pair(Name, Sequence));

	return true;
}

bool CAnimationManager3D::LoadAnimationSequence(bool Loop, const std::string& Name,
	const TCHAR* FileName, const std::string& PathName, class CScene* Scene)
{
	if (FindAnimationSequence(Name))
	{
		return true;
	}

	CAnimationSequence* Sequence = new CAnimationSequence;

	Sequence->m_Scene = Scene;

	if (!Sequence->CreateSequence(Loop, Name, FileName, PathName))
	{
		SAFE_DELETE(Sequence);
		return false;
	}

	Sequence->SetName(Name);

	m_mapSequence.insert(std::make_pair(Name, Sequence));

	return true;
}

bool CAnimationManager3D::LoadAnimationSequenceFullPath(bool Loop, const std::string& Name,
	const TCHAR* FullPath, class CScene* Scene)
{
	if (FindAnimationSequence(Name))
	{
		return true;
	}

	CAnimationSequence* Sequence = new CAnimationSequence;

	Sequence->m_Scene = Scene;

	if (!Sequence->CreateSequence(FullPath))
	{
		SAFE_DELETE(Sequence);
		return false;
	}

	Sequence->m_Loop = Loop;
	Sequence->SetName(Name);

	m_mapSequence.insert(std::make_pair(Name, Sequence));

	return true;
}

bool CAnimationManager3D::LoadAnimationSequenceMultibyte(bool Loop, const std::string& Name,
	const char* FileName, const std::string& PathName, class CScene* Scene)
{
	if (FindAnimationSequence(Name))
	{
		return true;
	}

	CAnimationSequence* Sequence = new CAnimationSequence;

	Sequence->m_Scene = Scene;

	char	FullPath[MAX_PATH] = {};

	const PathInfo* Info = CPathManager::GetInst()->FindPath(PathName);

	if (Info)
		strcpy_s(FullPath, Info->PathMultibyte);

	strcat_s(FullPath, FileName);

	if (!Sequence->CreateSequenceMultibyte(FullPath))
	{
		SAFE_DELETE(Sequence);
		return false;
	}

	Sequence->m_Loop = Loop;
	Sequence->SetName(Name);

	m_mapSequence.insert(std::make_pair(Name, Sequence));

	return true;
}

bool CAnimationManager3D::LoadAnimationSequenceFullPathMultibyte(bool Loop, const std::string& Name,
	const char* FullPath, class CScene* Scene)
{
	if (FindAnimationSequence(Name))
	{
		return true;
	}

	CAnimationSequence* Sequence = new CAnimationSequence;

	Sequence->m_Scene = Scene;

	if (!Sequence->CreateSequenceMultibyte(FullPath))
	{
		SAFE_DELETE(Sequence);
		return false;
	}

	Sequence->m_Loop = Loop;
	Sequence->SetName(Name);

	m_mapSequence.insert(std::make_pair(Name, Sequence));

	return true;
}

CAnimationSequence* CAnimationManager3D::FindAnimationSequence(const std::string& Name)
{
	auto	iter = m_mapSequence.find(Name);

	if (iter == m_mapSequence.end())
		return nullptr;

	return iter->second;
}

void CAnimationManager3D::ReleaseSequence(const std::string& Name)
{
	auto	iter = m_mapSequence.find(Name);

	if (iter != m_mapSequence.end())
	{
		if (iter->second->GetRefCount() == 1)
			m_mapSequence.erase(iter);
	}
}

bool CAnimationManager3D::LoadSkeleton(const std::string& Name, 
	const TCHAR* FileName, const std::string& PathName, CScene* Scene)
{
	TCHAR	FullPath[MAX_PATH] = {};

	const PathInfo* Info = CPathManager::GetInst()->FindPath(PathName);

	if (Info)
		lstrcpy(FullPath, Info->Path);

	lstrcat(FullPath, FileName);

	return LoadSkeletonFullPath(Name, FullPath, Scene);
}

bool CAnimationManager3D::LoadSkeletonFullPath(const std::string& Name,
	const TCHAR* FullPath, CScene* Scene)
{
	char	FullPathMultibyte[MAX_PATH] = {};

#ifdef UNICODE

	int	ConvertLength = WideCharToMultiByte(CP_ACP, 0, FullPath, -1, nullptr, 0, nullptr, nullptr);
	WideCharToMultiByte(CP_ACP, 0, FullPath, -1, FullPathMultibyte, ConvertLength, nullptr, nullptr);

#else

	strcpy_s(FullPathMultibyte, FullPath);

#endif // UNICODE

	return LoadSkeletonFullPathMultibyte(Name, FullPathMultibyte, Scene);
}

bool CAnimationManager3D::LoadSkeletonMultibyte(const std::string& Name, 
	const char* FileName, const std::string& PathName, CScene* Scene)
{
	if (FindSkeleton(Name))
	{
		return true;
	}

	CSkeleton* Skeleton = new CSkeleton;

	Skeleton->m_Scene = Scene;

	if (!Skeleton->LoadSkeleton(Scene, Name, FileName, PathName))
	{
		SAFE_DELETE(Skeleton);
		return false;
	}

	Skeleton->SetName(Name);

	m_mapSkeleton.insert(std::make_pair(Name, Skeleton));

	return true;
}

bool CAnimationManager3D::LoadSkeletonFullPathMultibyte(
	const std::string& Name, const char* FullPath, CScene* Scene)
{
	if (FindSkeleton(Name))
	{
		return true;
	}

	CSkeleton* Skeleton = new CSkeleton;

	Skeleton->m_Scene = Scene;

	if (!Skeleton->LoadSkeletonFullPath(Scene, Name, FullPath))
	{
		SAFE_DELETE(Skeleton);
		return false;
	}

	Skeleton->SetName(Name);

	m_mapSkeleton.insert(std::make_pair(Name, Skeleton));

	return true;
}

bool CAnimationManager3D::AddSocket(const std::string& SkeletonName, 
	const std::string& BoneName, const std::string& SocketName, 
	const Vector3& Offset, const Vector3& OffsetRot)
{
	CSkeleton* Skeleton = FindSkeleton(SkeletonName);

	if (!Skeleton)
		return false;

	Skeleton->AddSocket(BoneName, SocketName, Offset, OffsetRot);

	return true;
}

CSkeleton* CAnimationManager3D::FindSkeleton(const std::string& Name)
{
	auto	iter = m_mapSkeleton.find(Name);

	if (iter == m_mapSkeleton.end())
		return nullptr;

	return iter->second;
}

void CAnimationManager3D::ReleaseSkeleton(const std::string& Name)
{
	auto	iter = m_mapSkeleton.find(Name);

	if (iter != m_mapSkeleton.end())
	{
		if (iter->second->GetRefCount() == 1)
			m_mapSkeleton.erase(iter);
	}
}
