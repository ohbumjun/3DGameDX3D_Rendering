#pragma once

#include "../Thread.h"
#include "../ThreadQueue.h"
#include "../Component/SceneComponent.h"

class CNavigationThread3D :
    public CThread
{
	friend class CNavigation3DManager;

public:
	CNavigationThread3D();
	virtual ~CNavigationThread3D();

private:
	class CNavigation3DManager* m_NavManager;
	CThreadQueue<NavWorkData>			m_WorkQueue;
	class CNavigation3D* m_Navigation;
	HANDLE				m_ExitEvent;

public:
	int GetWorkCount()
	{
		return m_WorkQueue.size();
	}

	void AddNavMesh(class CNavigationMesh* NavMesh);

public:
	virtual void Run();

public:
	template <typename T, typename ComponentType>
	void AddWork(T* Obj, void(T::* Func)(const std::list<Vector3>&),
		ComponentType* OwnerComponent, const Vector3& End)
	{
		NavWorkData	Data;
		Data.Callback = std::bind(Func, Obj, std::placeholders::_1);
		//Data.Start = Start;
		Data.OwnerComponent = (CSceneComponent*)OwnerComponent;
		Data.End = End;

		m_WorkQueue.push(Data);
	}
};

