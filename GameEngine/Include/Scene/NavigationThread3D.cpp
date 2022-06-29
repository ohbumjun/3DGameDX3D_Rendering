
#include "NavigationThread3D.h"
#include "Navigation3D.h"
#include "Navigation3DManager.h"
#include "NavigationMesh.h"

CNavigationThread3D::CNavigationThread3D()
{
	SetLoop(true);

	m_Navigation = new CNavigation3D;

	m_ExitEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

CNavigationThread3D::~CNavigationThread3D()
{
	m_Loop = false;

	WaitForSingleObject(m_ExitEvent, INFINITE);

	CloseHandle(m_ExitEvent);

	SAFE_DELETE(m_Navigation);
}

void CNavigationThread3D::AddNavMesh(CNavigationMesh* NavMesh)
{
	m_Navigation->AddNavMesh(NavMesh);
}

void CNavigationThread3D::Run()
{
	while (m_Loop)
	{
		if (!m_WorkQueue.empty())
		{
			NavWorkData Work = m_WorkQueue.front();
			m_WorkQueue.pop();

			NavResultData	Result;

			m_Navigation->FindPath(Work.OwnerComponent->GetWorldPos(), Work.End, Result.vecPath);

			Result.Callback = Work.Callback;

			m_NavManager->AddNavResult(Result);
		}
	}

	SetEvent(m_ExitEvent);
}
