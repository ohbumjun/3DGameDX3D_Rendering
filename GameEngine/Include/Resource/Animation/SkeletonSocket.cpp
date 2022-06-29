
#include "SkeletonSocket.h"

CSkeletonSocket::CSkeletonSocket()
{
}

CSkeletonSocket::CSkeletonSocket(const CSkeletonSocket& Socket)
{
	*this = Socket;
}

CSkeletonSocket::~CSkeletonSocket()
{
}

void CSkeletonSocket::Update(const Matrix& matBone)
{
	Matrix	matOffsetRot, matOffset;
	matOffsetRot.Rotation(m_OffsetRot);
	matOffset.Translation(m_Offset);

	m_matSocket = matOffsetRot * matOffset * matBone;
}

CSkeletonSocket* CSkeletonSocket::Clone()
{
	return new CSkeletonSocket(*this);
}
