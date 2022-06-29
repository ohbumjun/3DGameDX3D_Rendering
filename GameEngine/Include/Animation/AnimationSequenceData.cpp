
#include "AnimationSequenceData.h"
#include "../Resource/Animation/AnimationSequence.h"

CAnimationSequenceData::CAnimationSequenceData() :
	m_Time(0.f),
	m_FrameTime(0.f),
	m_PlayTime(0.f),
	m_PlayScale(1.f),
	m_Loop(0)
{
}

CAnimationSequenceData::~CAnimationSequenceData()
{
	auto	iter = m_vecNotify.begin();
	auto	iterEnd = m_vecNotify.end();

	for (; iter != iterEnd; ++iter)
	{
		SAFE_DELETE((*iter));
	}
}

void CAnimationSequenceData::Save(FILE* File)
{
	int	Length = (int)m_Name.length();
	fwrite(&Length, sizeof(int), 1, File);
	fwrite(m_Name.c_str(), sizeof(char), Length, File);


	fwrite(&m_Time, sizeof(float), 1, File);
	fwrite(&m_FrameTime, sizeof(float), 1, File);
	fwrite(&m_PlayTime, sizeof(float), 1, File);
	fwrite(&m_PlayScale, sizeof(float), 1, File);

	fwrite(&m_Loop, sizeof(bool), 1, File);

	std::string	SequenceName = m_Sequence->GetName();

	Length = (int)SequenceName.length();
	fwrite(&Length, sizeof(int), 1, File);
	fwrite(SequenceName.c_str(), sizeof(char), Length, File);
}

void CAnimationSequenceData::Load(FILE* File)
{
	int	Length = 0;
	char	Name[256] = {};

	fread(&Length, sizeof(int), 1, File);
	fread(Name, sizeof(char), Length, File);
	m_Name = Name;


	fread(&m_Time, sizeof(float), 1, File);
	fread(&m_FrameTime, sizeof(float), 1, File);
	fread(&m_PlayTime, sizeof(float), 1, File);
	fread(&m_PlayScale, sizeof(float), 1, File);

	fread(&m_Loop, sizeof(bool), 1, File);

	Length = 0;
	char	SequenceName[256] = {};
	fread(&Length, sizeof(int), 1, File);
	fread(SequenceName, sizeof(char), Length, File);

	m_SequenceName = SequenceName;
}
