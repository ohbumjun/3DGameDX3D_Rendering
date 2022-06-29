
#include "Navigation3D.h"
#include "NavigationMesh.h"

CNavigation3D::CNavigation3D()
{
	m_ArriveDist = 0.f;
}

CNavigation3D::~CNavigation3D()
{
}

CNavigationMesh* CNavigation3D::FindNavMesh(const string& Name)
{
	for (size_t i = 0; i < m_vecNavMesh.size(); ++i)
	{
		if (m_vecNavMesh[i]->GetName() == Name)
			return m_vecNavMesh[i];
	}

	return nullptr;
}

CNavigationMesh* CNavigation3D::FindNavMesh(const Vector3& Pos)
{
	vector<class CNavigationMesh*>::iterator	iter1;
	vector<class CNavigationMesh*>::iterator	iter1End = m_vecNavMesh.end();

	for (iter1 = m_vecNavMesh.begin(); iter1 != iter1End; ++iter1)
	{
		CNavigationMesh* NavMesh = *iter1;

		Vector3	Min = NavMesh->GetMin() - 2.f;
		Vector3	Max = NavMesh->GetMax() + 2.f;

		if (Min.x <= Pos.x && Pos.x <= Max.x &&
			Min.y <= Pos.y && Pos.y <= Max.y &&
			Min.z <= Pos.z && Pos.z <= Max.z)
		{
			return NavMesh;
		}
	}

	return nullptr;
}

const vector<class CNavigationMesh*>* CNavigation3D::GetNavMesh()
{
	return &m_vecNavMesh;
}

float CNavigation3D::GetY(const Vector3& Pos)
{
	CNavigationMesh* pNavMesh = FindNavMesh(Pos);

	if (!pNavMesh)
		return false;

	float	fY = pNavMesh->GetY(Pos);

	return fY;
}

bool CNavigation3D::OnNavMesh(const Vector3& Pos)
{
	CNavigationMesh* pNavMesh = FindNavMesh(Pos);

	if (!pNavMesh)
		return false;

	return true;
}

bool CNavigation3D::CheckCell(const Vector3& Pos)
{
	CNavigationMesh* pNavMesh = FindNavMesh(Pos);

	if (!pNavMesh)
		return false;

	bool	bRet = pNavMesh->CheckCell(Pos);

	return bRet;
}

bool CNavigation3D::RayIntersectTriangle(Ray& tRay)
{
	CNavigationMesh* pNavMesh = m_vecNavMesh[0];

	if (!pNavMesh)
		return false;

	bool	bRet = pNavMesh->RayIntersectTriangle(tRay);

	return bRet;
}

bool CNavigation3D::FindPath(const Vector3& Start,
	const Vector3& End, std::list<Vector3>& vecPath)
{
	CNavigationMesh* pNavMesh = FindNavMesh(Start);

	if (!pNavMesh)
		return false;

	pNavMesh->FindPath(Start, End);

	vecPath = pNavMesh->GetPathList();

	m_ArriveDist = pNavMesh->GetArriveDist();

	return true;
}
