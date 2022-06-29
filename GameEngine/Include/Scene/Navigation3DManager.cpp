
#include "Navigation3DManager.h"
#include "NavigationMesh.h"
#include "../Input.h"
#include "CameraManager.h"
#include "Scene.h"
#include "../Component/CameraComponent.h"

CNavigation3DManager::CNavigation3DManager()
{
}

CNavigation3DManager::~CNavigation3DManager()
{
	size_t	Size = m_vecNavigationThread.size();

	for (size_t i = 0; i < Size; ++i)
	{
		SAFE_DELETE(m_vecNavigationThread[i]);
	}

	m_vecNavigationThread.clear();

	auto	iter = m_mapNavMesh.begin();
	auto	iterEnd = m_mapNavMesh.end();

	for (; iter != iterEnd; ++iter)
	{
		SAFE_DELETE(iter->second);
	}
}

CNavigationMesh* CNavigation3DManager::AddNavData(CLandScape* NavData)
{
	// LandScape를 이용해서 내비게이션 데이터를 만든다.
	CNavigationMesh* NavMesh = FindNavMesh(NavData->GetName());

	if (NavMesh)
		return nullptr;


	NavMesh = new CNavigationMesh;

	NavMesh->Init();
	NavMesh->SetName(NavData->GetName());

	NavData->CreateNavigationData(NavMesh);

	m_mapNavMesh.insert(std::make_pair(NavData->GetName(), NavMesh));

	return NavMesh;
}

void CNavigation3DManager::AddNavResult(const NavResultData& NavData)
{
	m_ResultQueue.push(NavData);
}

float CNavigation3DManager::GetY(const Vector3& Pos)
{
	CNavigationMesh* pNavMesh = FindNavMesh(Pos);

	if (!pNavMesh)
		return false;

	float	fY = pNavMesh->GetY(Pos);

	return fY;
}

Vector3 CNavigation3DManager::GetPickingPos() const
{
	Matrix matView = m_Scene->GetCameraManager()->GetCurrentCamera()->GetViewMatrix();

	Ray	tRay = CInput::GetInst()->GetRay(matView);
	tRay.ImpactPoint = Vector3::Zero;

	// 최적화를 하려면 DDT 같은 Picking 최적화 알고리즘을 구현해야 한다.
	auto	iter = m_mapNavMesh.begin();
	auto	iterEnd = m_mapNavMesh.end();

	for (; iter != iterEnd; ++iter)
	{
		if (iter->second->RayIntersectTriangle(tRay))
			break;
	}

	return tRay.ImpactPoint;
}

void CNavigation3DManager::Start()
{
	if (!m_vecNavigationThread.empty())
	{
		size_t	Size = m_vecNavigationThread.size();

		for (size_t i = 0; i < Size; ++i)
		{
			SAFE_DELETE(m_vecNavigationThread[i]);
		}

		m_vecNavigationThread.clear();
	}

	if (!m_mapNavMesh.empty())
	{
		for (int i = 0; i < 4; ++i)
		{
			char	Name[256] = {};
			sprintf_s(Name, "NavThread%d", i);
			CNavigationThread3D* Thread = CThread::CreateThread<CNavigationThread3D>(Name);

			Thread->m_NavManager = this;

			auto	iter = m_mapNavMesh.begin();
			auto	iterEnd = m_mapNavMesh.end();

			for (; iter != iterEnd; ++iter)
			{
				Thread->AddNavMesh(iter->second);
			}

			Thread->Start();

			m_vecNavigationThread.push_back(Thread);
		}
	}
}

bool CNavigation3DManager::Init()
{
	return true;
}

void CNavigation3DManager::Update(float DeltaTime)
{
	if (!m_ResultQueue.empty())
	{
		NavResultData	Result = m_ResultQueue.front();
		m_ResultQueue.pop();

		Result.Callback(Result.vecPath);
	}
}

CNavigationMesh* CNavigation3DManager::FindNavMesh(const std::string& Name)
{
	auto	iter = m_mapNavMesh.find(Name);

	if (iter == m_mapNavMesh.end())
		return nullptr;

	return iter->second;
}

CNavigationMesh* CNavigation3DManager::FindNavMesh(const Vector3& vPos)
{
	auto	iter1 = m_mapNavMesh.begin();
	auto	iter1End = m_mapNavMesh.end();

	for (; iter1 != iter1End; ++iter1)
	{
		CNavigationMesh* pNavMesh = iter1->second;

		Vector3	vMin = pNavMesh->GetMin() - 2.f;
		Vector3	vMax = pNavMesh->GetMax() + 2.f;

		if (vMin.x <= vPos.x && vPos.x <= vMax.x &&
			vMin.y <= vPos.y && vPos.y <= vMax.y &&
			vMin.z <= vPos.z && vPos.z <= vMax.z)
		{
			return pNavMesh;
		}
	}

	return nullptr;
}
