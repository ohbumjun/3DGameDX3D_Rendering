
#include "AnimationSequenceInstance.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneResource.h"
#include "../Component/AnimationMeshComponent.h"
#include "../Resource/Animation/AnimationSequence.h"
#include "../Resource/Shader/AnimationUpdateConstantBuffer.h"
#include "../Resource/Shader/AnimationUpdateShader.h"
#include "../Resource/Animation/Skeleton.h"
#include "../Resource/Shader/StructuredBuffer.h"
#include "../Device.h"

CAnimationSequenceInstance::CAnimationSequenceInstance() :
	m_Scene(nullptr),
	m_Owner(nullptr),
	m_PlayAnimation(true),
	m_GlobalTime(0.f),
	m_SequenceProgress(0.f),
	m_ChangeTimeAcc(0.f),
	m_ChangeTime(0.2f)
{
	SetTypeID<CAnimationSequenceInstance>();
}

CAnimationSequenceInstance::CAnimationSequenceInstance(const CAnimationSequenceInstance& Anim)
{
	m_PlayAnimation = Anim.m_PlayAnimation;

	m_GlobalTime = 0.f;
	m_SequenceProgress = 0.f;
	m_ChangeTimeAcc = 0.f;
	m_ChangeTime = Anim.m_ChangeTime;

	m_AnimationUpdateShader = Anim.m_AnimationUpdateShader;

	if (Anim.m_AnimationUpdateCBuffer)
		m_AnimationUpdateCBuffer = Anim.m_AnimationUpdateCBuffer->Clone();

	if (Anim.m_OutputBuffer)
		m_OutputBuffer = Anim.m_OutputBuffer->Clone();

	if (Anim.m_BoneBuffer)
		m_BoneBuffer = Anim.m_BoneBuffer->Clone();

	m_Skeleton = Anim.m_Skeleton;



	m_mapAnimation.clear();

	auto	iter = Anim.m_mapAnimation.begin();
	auto	iterEnd = Anim.m_mapAnimation.end();

	for (; iter != iterEnd; ++iter)
	{
		CAnimationSequenceData* Data = new CAnimationSequenceData;

		Data->m_Sequence = iter->second->m_Sequence;
		Data->m_Name = iter->second->m_Name;
		Data->m_Loop = iter->second->m_Loop;
		Data->m_PlayTime = iter->second->m_PlayTime;
		Data->m_PlayScale = iter->second->m_PlayScale;

		if (Anim.m_CurrentAnimation->m_Name == Data->m_Name)
			m_CurrentAnimation = Data;

		m_mapAnimation.insert(std::make_pair(iter->first, Data));
	}
}

CAnimationSequenceInstance::~CAnimationSequenceInstance()
{
	SAFE_DELETE(m_OutputBuffer);
	SAFE_DELETE(m_BoneBuffer);
	SAFE_DELETE(m_AnimationUpdateCBuffer);

	auto	iter = m_mapAnimation.begin();
	auto	iterEnd = m_mapAnimation.end();

	for (; iter != iterEnd; ++iter)
	{
		SAFE_DELETE(iter->second);
	}
}

void CAnimationSequenceInstance::SetSkeleton(CSkeleton* Skeleton)
{
	m_Skeleton = Skeleton;
}

void CAnimationSequenceInstance::AddAnimation(const std::string& SequenceName,
	const std::string& Name, bool Loop,
	float PlayTime, float PlayScale)
{
	CAnimationSequenceData* Anim = FindAnimation(Name);

	if (Anim)
		return;

	CAnimationSequence* Sequence = nullptr;

	if (m_Scene)
	{
		Sequence = m_Scene->GetResource()->FindAnimationSequence(SequenceName);
	}

	else
	{
		Sequence = CResourceManager::GetInst()->FindAnimationSequence(SequenceName);
	}

	if (!Sequence)
		return;

	Anim = new CAnimationSequenceData;

	Anim->m_Sequence = Sequence;
	Anim->m_Name = Name;
	Anim->m_Loop = Loop;
	Anim->m_PlayTime = PlayTime;
	Anim->m_PlayScale = PlayScale;
	Anim->m_FrameTime = PlayTime / Sequence->GetKeyFrameCount();

	if (m_mapAnimation.empty())
	{
		m_CurrentAnimation = Anim;
	}

	m_mapAnimation.insert(std::make_pair(Name, Anim));
}

void CAnimationSequenceInstance::AddAnimation(const TCHAR* FileName,
	const std::string& PathName, const std::string& Name,
	bool Loop, float PlayTime, float PlayScale)
{
	CAnimationSequenceData* Anim = FindAnimation(Name);

	if (Anim)
		return;

	char	FileNameMultibyte[256] = {};

	int	Length = WideCharToMultiByte(CP_ACP, 0, FileName, -1, 0, 0, 0, 0);
	WideCharToMultiByte(CP_ACP, 0, FileName, -1, FileNameMultibyte, Length, 0, 0);

	CAnimationSequence* Sequence = nullptr;

	if (m_Scene)
	{
		std::string	SequenceName;

		m_Scene->GetResource()->LoadAnimationSequenceMultibyte(Loop, SequenceName, FileNameMultibyte);
		Sequence = m_Scene->GetResource()->FindAnimationSequence(SequenceName);
	}

	else
	{
		std::string	SequenceName;

		CResourceManager::GetInst()->LoadAnimationSequenceMultibyte(Loop, SequenceName, FileNameMultibyte);
		Sequence = CResourceManager::GetInst()->FindAnimationSequence(SequenceName);
	}

	if (!Sequence)
		return;

	Anim = new CAnimationSequenceData;

	Anim->m_Sequence = Sequence;
	Anim->m_Name = Name;
	Anim->m_Loop = Loop;
	Anim->m_PlayTime = PlayTime;
	Anim->m_PlayScale = PlayScale;
	Anim->m_FrameTime = PlayTime / Sequence->GetKeyFrameCount();

	if (m_mapAnimation.empty())
	{
		m_CurrentAnimation = Anim;
	}

	m_mapAnimation.insert(std::make_pair(Name, Anim));
}

void CAnimationSequenceInstance::SetPlayTime(const std::string& Name, float PlayTime)
{
	CAnimationSequenceData* Anim = FindAnimation(Name);

	if (!Anim)
		return;

	Anim->m_PlayTime = PlayTime;
}

void CAnimationSequenceInstance::SetPlayScale(const std::string& Name, float PlayScale)
{
	CAnimationSequenceData* Anim = FindAnimation(Name);

	if (!Anim)
		return;

	Anim->m_PlayScale = PlayScale;
}

void CAnimationSequenceInstance::SetLoop(const std::string& Name, bool Loop)
{
	CAnimationSequenceData* Anim = FindAnimation(Name);

	if (!Anim)
		return;

	Anim->m_Loop = Loop;
}

void CAnimationSequenceInstance::SetCurrentAnimation(const std::string& Name)
{
	m_CurrentAnimation = FindAnimation(Name);

	if (!m_CurrentAnimation)
		return;

	m_CurrentAnimation->m_Time = 0.f;

	size_t Size = m_CurrentAnimation->m_vecNotify.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_CurrentAnimation->m_vecNotify[i]->Call = false;
	}
}

void CAnimationSequenceInstance::ChangeAnimation(const std::string& Name)
{
	if (m_CurrentAnimation->m_Name == Name)
		return;

	//m_CurrentAnimation->m_Time = 0.f;

	size_t	Size = m_CurrentAnimation->m_vecNotify.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_CurrentAnimation->m_vecNotify[i]->Call = false;
	}

	m_ChangeAnimation = FindAnimation(Name);
	m_ChangeAnimation->m_Time = 0.f;
}

bool CAnimationSequenceInstance::CheckCurrentAnimation(const std::string& Name)
{
	return m_CurrentAnimation->m_Name == Name;
}

void CAnimationSequenceInstance::Start()
{
	if (m_Scene)
	{
		m_AnimationUpdateShader = (CComputeShader*)m_Scene->GetResource()->FindShader("AnimationUpdateShader");
	}

	else
	{
		m_AnimationUpdateShader = (CComputeShader*)CResourceManager::GetInst()->FindShader("AnimationUpdateShader");
	}
	
	m_AnimationUpdateCBuffer = new CAnimationUpdateConstantBuffer;

	m_AnimationUpdateCBuffer->Init();

	m_vecBoneMatrix.resize(m_Skeleton->GetBoneCount());

	m_OutputBuffer = new CStructuredBuffer;

	m_OutputBuffer->Init("OutputBone", sizeof(Matrix),
		(unsigned int)m_Skeleton->GetBoneCount(), 0);

	m_BoneBuffer = new CStructuredBuffer;

	m_BoneBuffer->Init("OutputBone", sizeof(Matrix),
		(unsigned int)m_Skeleton->GetBoneCount(), 1, 
		D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS,
		D3D11_CPU_ACCESS_READ, false);
	/*m_BoneBuffer->Init("OutputBone", sizeof(Matrix),
		(unsigned int)m_Skeleton->GetBoneCount(), 1);*/

	/*D3D11_BUFFER_DESC	Desc = {};

	Desc.ByteWidth = sizeof(Matrix) * (unsigned int)m_Skeleton->GetBoneCount();
	Desc.Usage = D3D11_USAGE_STAGING;
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	CDevice::GetInst()->GetDevice()->CreateBuffer(&Desc, nullptr, &m_BoneDataBuffer);*/
}

bool CAnimationSequenceInstance::Init()
{
	return true;
}

void CAnimationSequenceInstance::Update(float DeltaTime)
{
	if (!m_Skeleton || !m_CurrentAnimation || !m_PlayAnimation || m_CurrentAnimation->m_Sequence->GetKeyFrameCount() == 0)
		return;

	m_GlobalTime += DeltaTime * m_CurrentAnimation->m_PlayScale;

	bool	Change = false;
	bool	ChangeEnd = false;
	bool	AnimEnd = false;

	if (m_ChangeAnimation)
	{
		Change = true;
		m_ChangeTimeAcc += DeltaTime;

		if (m_ChangeTimeAcc >= m_ChangeTime)
		{
			m_ChangeTimeAcc = m_ChangeTime;
			ChangeEnd = true;
		}
	}

	else
	{
		m_SequenceProgress = m_GlobalTime / m_CurrentAnimation->m_PlayTime;

		if (m_GlobalTime >= m_CurrentAnimation->m_PlayTime)
		{
			//m_GlobalTime -= m_CurrentAnimation->m_PlayTime;
			m_GlobalTime = m_CurrentAnimation->m_PlayTime;

			AnimEnd = true;
		}

		m_AnimationUpdateCBuffer->SetChangeAnimation(false);
		m_AnimationUpdateCBuffer->SetChangeRatio(0.f);
		m_AnimationUpdateCBuffer->SetChangeFrameCount(0);

		float	AnimationTime = m_GlobalTime + m_CurrentAnimation->m_Sequence->m_StartTime;

		int	StartFrame = 0;
		int	EndFrame = m_CurrentAnimation->m_Sequence->m_FrameLength;

		int	FrameIndex = (int)(AnimationTime / m_CurrentAnimation->m_Sequence->m_FrameTime);
		int	NextFrameIndex = FrameIndex + 1;

		if (FrameIndex >= m_CurrentAnimation->m_Sequence->m_FrameLength)
			FrameIndex = m_CurrentAnimation->m_Sequence->m_FrameLength - 1;

		if (NextFrameIndex >= EndFrame)
			NextFrameIndex = StartFrame;

		float	Ratio = (AnimationTime - m_CurrentAnimation->m_Sequence->m_FrameTime * FrameIndex) / m_CurrentAnimation->m_Sequence->m_FrameTime;

		m_AnimationUpdateCBuffer->SetFrameCount(EndFrame);
		m_AnimationUpdateCBuffer->SetCurrentFrame(FrameIndex);
		m_AnimationUpdateCBuffer->SetNextFrame(NextFrameIndex);
		m_AnimationUpdateCBuffer->SetRatio(Ratio);
		m_AnimationUpdateCBuffer->SetBoneCount((int)m_Skeleton->GetBoneCount());

		size_t	Size = m_CurrentAnimation->m_vecNotify.size();

		for (size_t i = 0; i < Size; ++i)
		{
			if (!m_CurrentAnimation->m_vecNotify[i]->Call &&
				m_CurrentAnimation->m_vecNotify[i]->Frame == FrameIndex)
			{
				m_CurrentAnimation->m_vecNotify[i]->Call = true;
				m_CurrentAnimation->m_vecNotify[i]->Function();
			}
		}

		if (AnimEnd)
		{
			if (m_CurrentAnimation->m_EndFunction)
				m_CurrentAnimation->m_EndFunction();

			if (m_CurrentAnimation->m_Loop)
			{
				m_GlobalTime = 0.f;
				size_t	Size = m_CurrentAnimation->m_vecNotify.size();

				for (size_t i = 0; i < Size; ++i)
				{
					m_CurrentAnimation->m_vecNotify[i]->Call = false;
				}
			}
		}
	}

	if (Change)
	{
		m_AnimationUpdateCBuffer->SetChangeRatio(m_ChangeTimeAcc / m_ChangeTime);
		m_AnimationUpdateCBuffer->SetChangeAnimation(true);
		m_AnimationUpdateCBuffer->SetChangeFrameCount(m_ChangeAnimation->m_Sequence->m_FrameLength);

		m_ChangeAnimation->m_Sequence->SetChangeShader();
	}

	m_AnimationUpdateCBuffer->SetRowIndex(m_Owner->GetInstanceID());
	
	m_OutputBuffer->SetShader();

	m_AnimationUpdateCBuffer->UpdateCBuffer();

	m_Skeleton->SetShader();

	m_BoneBuffer->SetShader();

	m_InstancingBoneBuffer->SetShader();

	m_CurrentAnimation->m_Sequence->SetShader();

	unsigned int GroupX = (unsigned int)m_Skeleton->GetBoneCount() / 256 + 1;

	m_AnimationUpdateShader->Excute(GroupX, 1, 1);


	m_InstancingBoneBuffer->ResetShader();

	m_BoneBuffer->ResetShader();

	m_CurrentAnimation->m_Sequence->ResetShader();

	m_Skeleton->ResetShader();

	m_OutputBuffer->ResetShader();

	if (Change)
	{
		m_ChangeAnimation->m_Sequence->ResetChangeShader();
	}

	if (ChangeEnd)
	{
		m_CurrentAnimation = m_ChangeAnimation;
		m_ChangeAnimation = nullptr;
		m_ChangeTimeAcc = 0.f;
		m_GlobalTime = 0.f;
	}
	
	// 구조화 버퍼에 있는 본 정보를 DataBuffer로 복사한다.
	//CDevice::GetInst()->GetContext()->CopyResource(m_BoneDataBuffer, m_BoneBuffer->GetBuffer());

	D3D11_MAPPED_SUBRESOURCE	Map = {};

	/*CDevice::GetInst()->GetContext()->Map(m_BoneDataBuffer, 0, D3D11_MAP_READ, 0, &Map);

	memcpy(&m_vecBoneMatrix[0], Map.pData, sizeof(Matrix)* m_vecBoneMatrix.size());;

	CDevice::GetInst()->GetContext()->Unmap(m_BoneDataBuffer, 0);*/

	CDevice::GetInst()->GetContext()->Map(m_BoneBuffer->GetBuffer(), 0, D3D11_MAP_READ, 0, &Map);

	memcpy(&m_vecBoneMatrix[0], Map.pData, sizeof(Matrix)* m_vecBoneMatrix.size());;

	CDevice::GetInst()->GetContext()->Unmap(m_BoneBuffer->GetBuffer(), 0);


	m_Skeleton->Update(DeltaTime, m_vecBoneMatrix, m_Owner->GetWorldMatrix());
}

void CAnimationSequenceInstance::SetShader()
{
	m_OutputBuffer->SetShader(106, (int)Buffer_Shader_Type::Vertex);
}

void CAnimationSequenceInstance::ResetShader()
{
	m_OutputBuffer->ResetShader(106, (int)Buffer_Shader_Type::Vertex);
}

CAnimationSequenceInstance* CAnimationSequenceInstance::Clone()
{
	return new CAnimationSequenceInstance(*this);
}

void CAnimationSequenceInstance::Save(FILE* File)
{
	int	AnimCount = (int)m_mapAnimation.size();
	fwrite(&AnimCount, sizeof(int), 1, File);

	auto	iter = m_mapAnimation.begin();
	auto	iterEnd = m_mapAnimation.end();

	for (; iter != iterEnd; ++iter)
	{
		int	Length = (int)iter->first.length();
		fwrite(&Length, sizeof(int), 1, File);
		fwrite(iter->first.c_str(), sizeof(char), Length, File);

		iter->second->Save(File);
	}

	const std::string& CurrentName = m_CurrentAnimation->GetName();
	int	Length = (int)CurrentName.length();
	fwrite(&Length, sizeof(int), 1, File);
	fwrite(CurrentName.c_str(), sizeof(char), Length, File);

	fwrite(&m_PlayAnimation, sizeof(bool), 1, File);
}

void CAnimationSequenceInstance::Load(FILE* File)
{
	int	AnimCount = 0;
	fread(&AnimCount, sizeof(int), 1, File);

	for (int i = 0; i < AnimCount; ++i)
	{
		int	Length = 0;
		char	AnimName[256] = {};

		fread(&Length, sizeof(int), 1, File);
		fread(AnimName, sizeof(char), Length, File);

		CAnimationSequenceData* Data = new CAnimationSequenceData;

		Data->Load(File);

		if (m_Scene)
		{
			Data->m_Sequence = m_Scene->GetResource()->FindAnimationSequence(Data->m_SequenceName);
		}

		else
		{
			Data->m_Sequence = CResourceManager::GetInst()->FindAnimationSequence(Data->m_SequenceName);
		}

		m_mapAnimation.insert(std::make_pair(AnimName, Data));
	}

	int	Length = 0;
	char	CurrentName[256] = {};

	fread(&Length, sizeof(int), 1, File);
	fread(CurrentName, sizeof(char), Length, File);

	m_CurrentAnimation = FindAnimation(CurrentName);

	fread(&m_PlayAnimation, sizeof(bool), 1, File);


	//if (m_Scene)
	//	m_CBuffer = m_Scene->GetResource()->GetAnimation2DCBuffer();
}

CAnimationSequenceData* CAnimationSequenceInstance::FindAnimation(const std::string& Name)
{
	auto	iter = m_mapAnimation.find(Name);

	if (iter == m_mapAnimation.end())
		return nullptr;

	return iter->second;
}
