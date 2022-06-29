#pragma once

#include "../GameInfo.h"
#include "../Component/LandScape.h"
#include "../ThreadQueue.h"
#include "NavigationThread3D.h"

class CNavigation3DManager
{
	friend class CScene;

private:
	CNavigation3DManager();
	~CNavigation3DManager();

private:
	class CScene* m_Scene;
	std::vector<CNavigationThread3D*>	m_vecNavigationThread;
	//CSharedPtr<CLandScape>				m_NavData;
	CThreadQueue<NavResultData>			m_ResultQueue;

	// 여러개의 NavMesh를 만들어서 저장해둔다.
	std::unordered_map<std::string, class CNavigationMesh*>	m_mapNavMesh;

public:
	class CNavigationMesh* AddNavData(CLandScape* NavData);
	void AddNavResult(const NavResultData& NavData);

public:
	float GetY(const Vector3& Pos);
	Vector3 GetPickingPos()	const;

public:
	void Start();
	bool Init();
	void Update(float DeltaTime);

private:
	class CNavigationMesh* FindNavMesh(const std::string& Name);
	class CNavigationMesh* FindNavMesh(const Vector3& vPos);

public:
	template <typename T, typename ComponentType>
	bool FindPath(T* Obj, void(T::* Func)(const std::list<Vector3>&),
		ComponentType* OwnerComponent, const Vector3& End)
	{
		if (m_vecNavigationThread.empty())
			return false;

		int	Count = m_vecNavigationThread[0]->GetWorkCount();
		int	WorkIndex = 0;

		size_t	Size = m_vecNavigationThread.size();

		for (size_t i = 1; i < Size; ++i)
		{
			if (Count > m_vecNavigationThread[i]->GetWorkCount())
			{
				Count = m_vecNavigationThread[i]->GetWorkCount();
				WorkIndex = (int)i;
			}
		}

		m_vecNavigationThread[WorkIndex]->AddWork<T>(Obj, Func, OwnerComponent, End);

		return true;
	}
};

