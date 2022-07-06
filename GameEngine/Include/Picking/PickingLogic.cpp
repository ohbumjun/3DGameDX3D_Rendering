#include "PickingLogic.h"
#include "../Component/CameraComponent.h"
#include "../Component/ColliderBox3D.h"
#include "../Component/ColliderSphere.h"
#include "../Component/LandScape.h"
#include "../GameObject/SkyObject.h"
#include "../Input.h"
#include "../Collision/Collision.h"
#include "../Scene/SceneManager.h"

bool CPickingLogic::Picking(CGameObject*& result)
{
	CCameraComponent* Camera = CSceneManager::GetInst()->GetScene()->GetCameraManager()->GetCurrentCamera();

	// CInput 에서 실시간 만들어주는 Ray 정보는 뷰 공간 상에서의 Ray
	// 아래의 함수를 통해서, View 행렬의 역행렬을 곱해준 Ray
	// 즉, World 공간 상의 Ray 를 가져다준다.
	// 왜냐하면, 애초에 실시간으로 구해준 Input 에서의 ray 정보가
	// 뷰에서의 ray 정보이기 때문이다.
	Ray	ray = CInput::GetInst()->GetRay(Camera->GetViewMatrix());

	const std::list<class CSceneComponent*>& RenderCompLists = CSceneManager::GetInst()->GetScene()->GetRenderComponentsList();

	auto	iter = RenderCompLists.begin();
	auto	iterEnd = RenderCompLists.end();

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

void CPickingLogic::Bresenham(int stR, int stC, int edR, int edC, std::vector<std::pair<int, int>>& vecIdxP)
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
		vecIdxP.push_back(std::make_pair(x, y));
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

bool CPickingLogic::DDTPicking(CGameObject* LandScapeObject, CGameObject* Player, Vector3& PickedWorldPos)
{
	// LandScape Object 가 아니라면 return
	CLandScape* LandScapeComponent = dynamic_cast<CLandScape*>(LandScapeObject->GetRootComponent());

	if (LandScapeComponent == nullptr)
		return false;

	// >> 1. Ray 을 World Space 로 보내준다. (Ray ~ LandScape => World Space 에서 비교해줄 것이다.)
	CCameraComponent* Camera = CSceneManager::GetInst()->GetScene()->GetCameraManager()->GetCurrentCamera();
	Ray	ray = CInput::GetInst()->GetRay(Camera->GetViewMatrix());

	// >> 2. 지형 xz 에 투영한다. (ray의 위치는 y pos 를 0 에 세팅, Dir 의 경우, xz 정보만 고려)
	// Ray 출발점
	// Vector3 RayOnLandScapeY = Vector3(ray.Pos.x, LandScapeWorldPos.y, ray.Pos.z) + Vector3(ray.Dir.x, 0.f, ray.Dir.z);
	Vector3 RayStPosOnLandScapeY = Vector3(ray.Pos.x, 0.f, ray.Pos.z);

	// x,z 평면 상에서의 직선 방향
	Vector3 RayDirWithYZero = Vector3(ray.Dir.x, 0.f, ray.Dir.z);

	// 검사 시작점, 끝점을 찾는다. -> 두 직선 교차 알고리즘 활용하기
	Vector3 RayFinalCheckStPos, RayFinalCheckEdPos;

	// LandScape 의 경우, 왼쪽 위에서 아래로 내려오는 형태로 이루어져 있다.
	// 어떤 조치도 취해주지 않는다면, min은 0이 되고, max 는 크기만큼 세팅되어 있을 것이다.
	// 즉, WorldPos 를 사각형 왼쪽 하단에 둔다고 생각하면 된다.
	Vector3 LandScapeMin = LandScapeComponent->GetMin() * LandScapeComponent->GetRelativeScale() + LandScapeComponent->GetWorldPos();
	LandScapeMin.y = 0.f;

	Vector3 LandScapeMax = LandScapeComponent->GetMax() * LandScapeComponent->GetRelativeScale() + LandScapeComponent->GetWorldPos();
	LandScapeMax.y = 0.f;

	// Ray 직선과 , 사각형 4개 변 사이의 교차점을 구해야 한다.
	// 먼저 Ray 가 가는 방향의 End 지점을 구해야 한다. 해당 지점 ~ Ray Start 지점을 이어서 하나의 직선을
	// 정의할 수 있기 때문이다.
	// RayStart 에서 RayDir 방향으로 길게 늘여서 임의의 한점을 잡는다.
	// LandScape 크기의 * 10배만큼 늘여서 잡을 것 (적어도 LandScape 범위 밖에 위치해야 한다.)
	Vector3 TempRayCheckEdPos = RayStPosOnLandScapeY + RayDirWithYZero * LandScapeComponent->GetMeshSize() * LandScapeComponent->GetRelativeScale() * 10.f;
	TempRayCheckEdPos.y = 0.f;
	
	std::vector<Vector3> vecIntersects;

	// 아래 가로
	GetIntersectPoints(LandScapeMin, Vector3(LandScapeMax.x, 0.f, LandScapeMin.z), 
		RayStPosOnLandScapeY, TempRayCheckEdPos, vecIntersects);
	// 왼쪽 세로
	GetIntersectPoints(LandScapeMin, Vector3(LandScapeMin.x, 0.f, LandScapeMax.z), 
		RayStPosOnLandScapeY, TempRayCheckEdPos, vecIntersects);
	// 위쪽 가로
	GetIntersectPoints(Vector3(LandScapeMin.x, 0.f, LandScapeMax.z), LandScapeMax, 
		RayStPosOnLandScapeY, TempRayCheckEdPos, vecIntersects);
	// 오른쪽 세로 
	GetIntersectPoints(Vector3(LandScapeMax.x, 0.f, LandScapeMin.z), LandScapeMax,
		RayStPosOnLandScapeY, TempRayCheckEdPos, vecIntersects);

	// 찾아낸 교차점이 LandScape 밖에 있는지 확인한다.
	auto iterSt = vecIntersects.begin();
	auto iterEd = vecIntersects.end();

	for (; iterSt != iterEd;)
	{
		if ((*iterSt).x < LandScapeMin.x || (*iterSt).z < LandScapeMin.z || 
			(*iterSt).x > LandScapeMax.x || (*iterSt).z > LandScapeMax.z)
		{
			vecIntersects.erase(iterSt);
		}
		++iterSt;
	}

	// 1) Ray  시작점이 LandScape 안에 있다면, 1개의 교차점이 나와야 하고, 그것이 바로 End
	// 2) Ray 시작점이 LandScape 밖에 있다면, 정상적인 경우, 교차점 2개가 다 나와야 한다.
	// 3) 그외의 경우 -> Ray가 LandScape 를 지나지 않는다는 것이다.
	if (vecIntersects.size() <= 0)
		return false;

	bool IsRayStInsideLandScape = false;

	if (RayStPosOnLandScapeY.x >= LandScapeMin.x && RayStPosOnLandScapeY.z >= LandScapeMin.z &&
		RayStPosOnLandScapeY.x <= LandScapeMax.x && RayStPosOnLandScapeY.z <= LandScapeMax.z)
	{
		// LandScape 안쪽에 Ray의 Start 위치가 있다는 것이다.
		IsRayStInsideLandScape = true;
		RayFinalCheckStPos = RayStPosOnLandScapeY;
	}

	// Ray Start 가 LandScape 안쪽에 존재한다면, 교차점 1개
	if (IsRayStInsideLandScape == true)
	{
		// 여기 걸리면 안되는 것 아닌가 ?
		if (vecIntersects.size() != 1)
			return false;

		RayFinalCheckEdPos = vecIntersects[0];
	}
	
	// Ray Start 가 LandScape밖에 존재한다면 -> 교차점 2개
	else
	{
		// 마찬가지로 여기 걸리면 안되는 것 아닌가 ?
		if (vecIntersects.size() != 2)
			return false;

		// X 값이 더 작은 것을, St P 로 세팅할 것이다.
		if (vecIntersects[0].x < vecIntersects[1].x)
		{
			RayFinalCheckStPos = vecIntersects[0];
			RayFinalCheckEdPos = vecIntersects[1];
		}
		else
		{
			RayFinalCheckStPos = vecIntersects[1];
			RayFinalCheckEdPos = vecIntersects[0];
		}
	}

	// >> 3. Bresenham 알고리즘을 이용해서, 해당 Ray가 지나가는 LandScape 격자 영역 목록을 뽑아낸다
	// St, Ed 지점에 대해서 LandScape 상의 2차원 좌표값을 구한다.
	// 예를 들어, CountX,Z 가 각각 129 개라면
	// 실제 사각형 개수는 각각 128, 128개이다.
	float LandScapeSizeX = (LandScapeMax.x - LandScapeMin.x) / (float)(LandScapeComponent->GetCountX() - 1);
	float LandScapeSizeZ = (LandScapeMax.z - LandScapeMin.z) / (float)(LandScapeComponent->GetCountZ() - 1);

	int RayStartXIdx = (int)((RayFinalCheckStPos.x - LandScapeMin.x) / LandScapeSizeX);
	int RayStartZIdx = (int)((RayFinalCheckStPos.z - LandScapeMin.z) / LandScapeSizeZ);

	int RayEndXIdx = (int)((RayFinalCheckEdPos.x - LandScapeMin.x) / LandScapeSizeX);
	int RayEndZIdx = (int)((RayFinalCheckEdPos.z - LandScapeMin.z) / LandScapeSizeZ);

	// LandScape 4개의 변 각각에 대한 직선 방정식을 정의한다.
	std::vector<std::pair<int, int>> vecLandScapeRayGoingThroughIdx;
	vecLandScapeRayGoingThroughIdx.reserve((size_t)LandScapeComponent->GetCountX() * (size_t)LandScapeComponent->GetCountZ());
	
	Bresenham(RayStartXIdx, RayStartZIdx, RayEndXIdx, RayEndZIdx, vecLandScapeRayGoingThroughIdx);

	// 혹시나 지나가는 Idx 정보가 존재하지 않는다면 Return -> 여기에 걸리면 안되는 것 아닌가 ?
	if (vecLandScapeRayGoingThroughIdx.size() <= 0)
		return false;

	// >> 4. 해당 격자 안에 있는 삼각형 목록을 뽑아낸다.
	// vecRayGoingThroughIdx 안에 들어있는 Idx 정보는, 왼쪽 하단 -> 오른쪽 상단 방향으로 증가하는
	// LandScape 사각형 단위의 Idx 정보이다.

	// vecTriangleRayGoingThroughIdx 의 경우, 왼쪽 상단 -> 오른쪽 하단 방향으로 내려오는 삼각형 판별 기준 Idx
	// 삼각형 - Ray 까지 거리 , 삼각형 Idx . 에 대한 정보들을 담을 것이다.
	// 차후, 삼각형 - Ray 까지 거리  를 기준으로 오름차순 정렬해주기 위해서, "삼각형 - Ray 까지 거리" 를 먼저 담는다.
	std::vector<int> vecTriangleRayGoingThroughIdx;

	for (const auto &[XIdx, ZIdx] : vecLandScapeRayGoingThroughIdx)
	{ 
		// Ray	ray = CInput::GetInst()->GetRay(Camera->GetViewMatrix());
		auto IntersectResult = LandScapeComponent->CheckRayIntersectsTriangle(XIdx, ZIdx, ray.Pos, ray.Dir);

		if (IntersectResult.has_value())
		{
			float DistToTriangle = IntersectResult.value();
			vecTriangleRayGoingThroughIdx.push_back(DistToTriangle);
		}
	}

	// 4. 해당 삼각형 목록 중에서 Ray 와 가장 가까이 위치한 삼각형 정보를 뽑아낸다.
	// 혹시나 충돌하는 삼각형 정보가 없다면 Break -> 이것은 가능성 있다. Bounding Volume 과만 부딪힐 뿐
	// 실제 Traingle 들과는 부딪히지 않을 수도 있기 때문이다.
	if (vecLandScapeRayGoingThroughIdx.size() <= 0)
		return false;

	float DistToTriangle = vecLandScapeRayGoingThroughIdx[0].second;

	PickedWorldPos = ray.Pos + ray.Dir * DistToTriangle;

	return true;
}

// https://devvcxxx.tistory.com/38
bool CPickingLogic::GetIntersectPoints(const Vector3& StartPoint, const Vector3& EndPoint, 
	const Vector3& RayStartPos, const Vector3& RayEndPos,
	std::vector<Vector3>& vecIntersects)
{
	// BP1 : StartPoint, 
	// BP2 : EndPoint
	// AP1 : RayStartPos
	// AP2 : RayEndPos
	float t;
	float s;

	// 분모
	// double under = (BP2.y - BP1.y) * (AP2.x - AP1.x) - (BP2.x - BP1.x) * (AP2.y - AP1.y);
	float under = (EndPoint.z - StartPoint.z) * (RayEndPos.x - RayStartPos.x) - (EndPoint.x - StartPoint.x) * (RayEndPos.z - RayStartPos.z);
	
	// 분모값이 0 이라는 의미는 두 선이 평행하다는 의미이다.
	if (under == 0)
		return false;

	// double _t = (BP2.x - BP1.x) * (AP1.y - BP1.y) - (BP2.y - BP1.y) * (AP1.x - BP1.x);
	// double _s = (AP2.x - AP1.x) * (AP1.y - BP1.y) - (AP2.y - AP1.y) * (AP1.x - BP1.x);
	float _t = (EndPoint.x - StartPoint.x) * (RayStartPos.z - StartPoint.z) - (EndPoint.z - StartPoint.z) * (RayStartPos.x - StartPoint.x);
	float _s = (RayEndPos.x - RayStartPos.x) * (RayStartPos.z - StartPoint.z) - (RayEndPos.z - RayStartPos.z) * (RayStartPos.x - StartPoint.x);

	// t, s 가 바로, 두 직선의 교차점이다.
	t = _t / under;
	s = _s / under;

	// t, s 는 0에서 1사이의 값어야만, 두 직선 영역 사이에 교점이 존재한다는 의미이다.
	if (t < 0.0 || t>1.0 || s < 0.0 || s>1.0)
		return false;

	// 분자값이 둘다 0이라는 의미는, 두 직선이 같은 직선이라는 의미이다.
	if (_t == 0 && _s == 0)
		return false;

	float IntersectX = RayStartPos.x + t * (float)(RayEndPos.x - RayStartPos.x);
	float IntersectZ = RayStartPos.z + t * (float)(RayEndPos.z - RayStartPos.z);

	vecIntersects.push_back(Vector3(IntersectX, 0.f, IntersectZ));

	return true;
}
