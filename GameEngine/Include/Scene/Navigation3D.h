#pragma once

#include "../GameInfo.h"

class CNavigation3D
{
	friend class CNavigationThread3D;

private:
	CNavigation3D();
	~CNavigation3D();

private:
	std::vector<class CNavigationMesh*>	m_vecNavMesh;
	float	m_ArriveDist;	// µµÂø°Å¸®

public:
	float GetArriveDist()	const
	{
		return m_ArriveDist;
	}

public:
	void AddNavMesh(class CNavigationMesh* NavMesh)
	{
		m_vecNavMesh.push_back(NavMesh);
	}

	class CNavigationMesh* FindNavMesh(const std::string& Name);
	class CNavigationMesh* FindNavMesh(const Vector3& Pos);
	const std::vector<class CNavigationMesh*>* GetNavMesh();
	float GetY(const Vector3& Pos);
	bool OnNavMesh(const Vector3& Pos);
	bool CheckCell(const Vector3& Pos);
	bool RayIntersectTriangle(Ray& tRay);
	virtual bool FindPath(const Vector3& Start,
		const Vector3& End, std::list<Vector3>& vecPath);
};

