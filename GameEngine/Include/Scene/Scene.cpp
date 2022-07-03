
#include "Scene.h"
#include "../PathManager.h"
#include "SceneManager.h"
#include "../Component/CameraComponent.h"
#include "../Component/ColliderBox3D.h"
#include "../Component/ColliderSphere.h"
#include "../Component/LandScape.h"
#include "../GameObject/SkyObject.h"
#include "../Input.h"
#include "../Collision/Collision.h"

CScene::CScene()
{
	m_Mode = new CSceneMode;
	m_Resource = new CSceneResource;
	m_Collision = new CSceneCollision;
	m_CameraManager = new CCameraManager;
	m_Viewport = new CViewport;
	m_NavManager = new CNavigationManager;
	m_Nav3DManager = new CNavigation3DManager;
	m_LightManager = new CLightManager;

	m_Mode->m_Scene = this;
	m_Resource->m_Scene = this;
	m_Collision->m_Scene = this;
	m_CameraManager->m_Scene = this;
	m_Viewport->m_Scene = this;
	m_NavManager->m_Scene = this;
	m_Nav3DManager->m_Scene = this;
	m_LightManager->m_Scene = this;

	m_Start = false;

	m_Collision->Init();
	m_CameraManager->Init();
	m_Viewport->Init();
	m_NavManager->Init();
	m_Nav3DManager->Init();
	m_LightManager->Init();

	m_Change = true;

	m_SkyObject = new CSkyObject;

	m_SkyObject->SetName("Sky");
	m_SkyObject->SetScene(this);

	m_SkyObject->Init();
}

CScene::~CScene()
{
	m_ObjList.clear();
	SAFE_DELETE(m_Nav3DManager);
	SAFE_DELETE(m_NavManager);
	SAFE_DELETE(m_Viewport);
	SAFE_DELETE(m_CameraManager);
	SAFE_DELETE(m_Collision);
	SAFE_DELETE(m_Resource);
	m_LightManager->Destroy();
	SAFE_DELETE(m_LightManager);
}

bool CScene::Picking(CGameObject*& result)
{
	CCameraComponent* Camera = m_CameraManager->GetCurrentCamera();

	// CInput 에서 실시간 만들어주는 Ray 정보는 뷰 공간 상에서의 Ray
	// 아래의 함수를 통해서, View 행렬의 역행렬을 곱해준 Ray
	// 즉, World 공간 상의 Ray 를 가져다준다.
	// 왜냐하면, 애초에 실시간으로 구해준 Input 에서의 ray 정보가
	// 투영공간에서의 ray 정보이기 때문이다.
	Ray	ray = CInput::GetInst()->GetRay(Camera->GetViewMatrix());

	auto	iter = m_RenderComponentList.begin();
	auto	iterEnd = m_RenderComponentList.end();

	Vector3	HitPoint;

	for (; iter != iterEnd; ++iter)
	{
		SphereInfo	Info = (*iter)->GetSphereInfo();

		if (CCollision::CollisionRayToSphere(HitPoint, ray, Info))
		{
			result = (*iter)->GetGameObject();
			return true;
		}
	}

	result = nullptr;

	return false;
}

void CScene::Bresenham(int stR, int stC, int edR, int edC, std::vector<std::pair<int, int>>& vecP)
{
	// 가로 
	int x = stC;
	// 세로 
	int y = stR;

	int dx = edC - stC;
	int dy = edR - stR;

	int detP = 2 * dy - dx;

	while (x <= edC)
	{
		vecP.push_back(std::make_pair(x, y));
		++x;

		if (detP < 0)
			detP = detP + 2 * dy;
		else
		{
			detP = detP + 2 * dy - 2 * dx;
			y++;
		}
	}
}

void CScene::DDTPicking(CGameObject* LandScapeObject, CGameObject* Player)
{
	// LandScape Object 가 아니라면 return
	CLandScape* LandScapeComponent = dynamic_cast<CLandScape*>(LandScapeObject->GetRootComponent());

	if (LandScapeComponent == nullptr)
		return;

	// >> 1. Ray 을 World Space 로 보내준다. (Ray ~ LandScape => World Space 에서 비교해줄 것이다.)
	CCameraComponent* Camera = m_CameraManager->GetCurrentCamera();
	Ray	ray = CInput::GetInst()->GetRay(Camera->GetViewMatrix());

	// >> 2. 지형 xz 에 투영한다. (ray의 위치는 y pos 를 LandScape WorldPos 에 세팅, Dir 의 경우, xz 정보만 고려)
	const Vector3& LandScapeWorldPos = LandScapeObject->GetWorldPos();

	// Ray 출발점
	// Vector3 RayOnLandScapeY = Vector3(ray.Pos.x, LandScapeWorldPos.y, ray.Pos.z) + Vector3(ray.Dir.x, 0.f, ray.Dir.z);
	Vector3 RayOnLandScapeY = Vector3(ray.Pos.x, LandScapeWorldPos.y, ray.Pos.z);

	// 검사 시작점, 끝점을 찾는다. -> 두 직선 교차 알고리즘 활용하기
	Vector3 RayCheckStPos, RayCheckEdPos;

	// LandScape 의 경우, 왼쪽 위에서 아래로 내려오는 형태로 이루어져 있다.
	// 어떤 조치도 취해주지 않는다면, min은 0이 되고, max 는 크기만큼 세팅되어 있을 것이다.
	// 즉, WorldPos 를 사각형 왼쪽 하단에 둔다고 생각하면 된다.
	// 출발점이 LandScape 안에 있지 않다면 
	SphereInfo  LandScapeSphereInfo = LandScapeComponent->GetSphereInfo();
	
	Vector3 LandScapeMin = LandScapeSphereInfo.Center - (LandScapeComponent->GetMeshSize() * LandScapeComponent->GetRelativeScale());
	LandScapeMin.y = LandScapeWorldPos.y;

	Vector3 LandScapeMax = LandScapeSphereInfo.Center + (LandScapeComponent->GetMeshSize() * LandScapeComponent->GetRelativeScale());
	LandScapeMax.y = LandScapeWorldPos.y;

	// LandScape 4개의 변 각각에 대한 직선 방정식을 정의한다.


	bool IsRayStInsideLandScape = false;

	if (RayOnLandScapeY.x >= LandScapeMin.x && RayOnLandScapeY.z >= LandScapeMin.z &&
		RayOnLandScapeY.x <= LandScapeMax.x && RayOnLandScapeY.z <= LandScapeMax.z)
	{
		IsRayStInsideLandScape = true;
	}

	// >> 3. Bresenham 알고리즘을 이용해서, 해당 Ray가 지나가는 LandScape 격자 영역 목록을 뽑아낸다

	// >> 4. 해당 격자 안에 있는 삼각형 목록을 뽑아낸다.

	// 4. 해당 삼각형 목록 중에서 Ray 와 가장 가까이 위치한 삼각형 정보를 뽑아낸다.

}

void CScene::Start()
{
	m_Mode->Start();

	auto	iter = m_ObjList.begin();
	auto	iterEnd = m_ObjList.end();

	for (; iter != iterEnd; ++iter)
	{
		(*iter)->Start();
	}

	m_Start = true;

	m_SkyObject->Start();

	m_CameraManager->Start();
	m_Collision->Start();
	m_Viewport->Start();
	m_LightManager->Start();

	if (m_Mode->GetPlayerObject())
	{
		CCameraComponent* Camera = m_Mode->GetPlayerObject()->FindComponentFromType<CCameraComponent>();

		if (Camera)
		{
			m_CameraManager->SetCurrentCamera(Camera);
		}
	}

	m_NavManager->Start();

	m_Nav3DManager->Start();
}

void CScene::Update(float DeltaTime)
{
	m_Mode->Update(DeltaTime);

	//m_SkyObject->Update(DeltaTime);

	auto	iter = m_ObjList.begin();
	auto	iterEnd = m_ObjList.end();

	for (; iter != iterEnd;)
	{
		if (!(*iter)->IsActive())
		{
			iter = m_ObjList.erase(iter);
			iterEnd = m_ObjList.end();
			continue;
		}

		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		(*iter)->Update(DeltaTime);
		++iter;
	}

	m_CameraManager->Update(DeltaTime);

	m_Viewport->Update(DeltaTime);

	m_NavManager->Update(DeltaTime);

	m_Nav3DManager->Update(DeltaTime);

	m_LightManager->Update(DeltaTime);
}

void CScene::PostUpdate(float DeltaTime)
{
	m_Mode->PostUpdate(DeltaTime);

	m_SkyObject->PostUpdate(DeltaTime);

	auto	iter = m_ObjList.begin();
	auto	iterEnd = m_ObjList.end();

	for (; iter != iterEnd;)
	{
		if (!(*iter)->IsActive())
		{
			iter = m_ObjList.erase(iter);
			iterEnd = m_ObjList.end();
			continue;
		}

		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		(*iter)->PostUpdate(DeltaTime);
		++iter;
	}

	m_CameraManager->PostUpdate(DeltaTime);

	m_Viewport->PostUpdate(DeltaTime);

	// 충돌체들을 충돌 영역에 포함시킨다.
	iter = m_ObjList.begin();
	iterEnd = m_ObjList.end();

	m_RenderComponentList.clear();

	for (; iter != iterEnd; ++iter)
	{
		// Culling 여부 조사하고
		(*iter)->AddCollision();

		// 해당 Object 의 모든 Scene Component 들을 돌면서
		const std::list<CSceneComponent*>& List = (*iter)->GetSceneComponents();

		auto	iter1 = List.begin();
		auto	iter1End = List.end();

		for (; iter1 != iter1End; ++iter1)
		{
			// ColliderBox3D, ColliderSphere 의 경우, Picking 대상으로 고려하지 않을 것이다.
			if ((*iter1)->IsIgnorePicking())
				continue;

			// Render 되기도 하면서, Culling 처리 되지 않은 녀석들을 모아둔다.
			if ((*iter1)->GetRender() && !(*iter1)->GetCulling())
			{
				m_RenderComponentList.push_back(*iter1);
			}
		}
	}

	// 출력되는 물체를 정렬한다.
	if (m_RenderComponentList.size() >= 2)
	{
		m_RenderComponentList.sort(SortRenderList);
	}

	// 포함된 충돌체들을 이용해서 충돌처리를 진행한다.
	m_Collision->Collision(DeltaTime);
}

void CScene::Save(const char* FileName, const std::string& PathName)
{
	const PathInfo* Info = CPathManager::GetInst()->FindPath(PathName);

	char	FullPath[MAX_PATH] = {};

	if (Info)
		strcpy_s(FullPath, Info->PathMultibyte);

	strcat_s(FullPath, FileName);

	SaveFullPath(FullPath);
}

void CScene::SaveFullPath(const char* FullPath)
{
	FILE* File = nullptr;

	fopen_s(&File, FullPath, "wb");

	if (!File)
		return;

	size_t	SceneModeType = m_Mode->GetTypeID();

	fwrite(&SceneModeType, sizeof(size_t), 1, File);

	size_t	ObjCount = m_ObjList.size();

	fwrite(&ObjCount, sizeof(size_t), 1, File);

	auto	iter = m_ObjList.begin();
	auto	iterEnd = m_ObjList.end();

	for (; iter != iterEnd; ++iter)
	{
		size_t	ObjType = (*iter)->GetTypeID();

		fwrite(&ObjType, sizeof(size_t), 1, File);

		(*iter)->Save(File);
	}


	fclose(File);
}

void CScene::Load(const char* FileName, const std::string& PathName)
{
	const PathInfo* Info = CPathManager::GetInst()->FindPath(PathName);

	char	FullPath[MAX_PATH] = {};

	if (Info)
		strcpy_s(FullPath, Info->PathMultibyte);

	strcat_s(FullPath, FileName);

	LoadFullPath(FullPath);
}

void CScene::LoadFullPath(const char* FullPath)
{
	FILE* File = nullptr;

	fopen_s(&File, FullPath, "rb");

	if (!File)
		return;

	m_ObjList.clear();

	size_t	SceneModeType = 0;

	fread(&SceneModeType, sizeof(size_t), 1, File);

	// SceneMode 생성
	CSceneManager::GetInst()->CallCreateSceneMode(this, SceneModeType);

	size_t	ObjCount = m_ObjList.size();

	fread(&ObjCount, sizeof(size_t), 1, File);

	for (size_t i = 0; i < ObjCount; ++i)
	{
		size_t	ObjType = 0;
		fread(&ObjType, sizeof(size_t), 1, File);

		CGameObject* Obj = CSceneManager::GetInst()->CallCreateObject(this, ObjType);

		Obj->Load(File);
	}

	fclose(File);
}

bool CScene::SortRenderList(CSceneComponent* Src, CSceneComponent* Dest)
{
	SphereInfo	SrcInfo = Src->GetSphereInfoViewSpace();
	SphereInfo	DestInfo = Dest->GetSphereInfoViewSpace();

	return SrcInfo.Center.Length() - SrcInfo.Radius > DestInfo.Center.Length() - DestInfo.Radius;
}
