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

// 기울기가 양수일때만 고려한다.
// 일반 Bresenham은 일반적으로 x가 1증가
// 특정 판별식을 이용해서 x가 1 증가할 때, y는 그대로 머물러 있을지, y도 1증가할지를 판정
void CPickingLogic::Bresenham(int stR, int stC, int edR, int edC, std::vector<std::pair<int, int>>& vecIdxP)
{
	// 기울기가 1이하인지, 1보다 큰지에 따라
	// detP 를 구하는 방식이 다르다.

	// 가로 (열)
	int C = stC;
	// 세로 (행)
	int R = stR;

	int dC = edC - stC;
	int dR = edR - stR;

	// 기울기가 1 이하인 경우라면
	if (dR <= dC)
	{
		int detP = 2 * (dR - dC);

		while (C <= edC)
		{
			// 가로, 세로 (행, 열)
			// 즉, (z,x) 정보를 반환한다.
			vecIdxP.push_back(std::make_pair(R, C));
			++C;

			if (detP < 0)
				detP = detP + 2 * dR;
			else
			{
				detP = detP + 2 * (dR - dC);
				R++;
			}
		}
	}
	// 기울기가 1 보다 큰 경우라면
	else
	{
		int detP = 2 * (dC - dR);

		while (R <= edR)
		{
			// 가로, 세로 (행, 열)
			// 즉, (z,x) 정보를 반환한다.
			vecIdxP.push_back(std::make_pair(R, C));
			++R;

			if (detP < 0)
				detP = detP + 2 * dC;
			else
			{
				detP = detP + 2 * (dC - dR);
				C++;
			}
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
	ray.Dir.Normalize();
		
	// >> 2. 지형 xz 에 투영한다. (ray의 위치는 y pos 를 0 에 세팅, Dir 의 경우, xz 정보만 고려)
	// Ray 출발점
	// Vector3 RayOnLandScapeY = Vector3(ray.Pos.x, LandScapeWorldPos.y, ray.Pos.z) + Vector3(ray.Dir.x, 0.f, ray.Dir.z);
	Vector3 RayStPosWithYZero = Vector3(ray.Pos.x, 0.f, ray.Pos.z);

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
	// LandScape 크기의 * 1000배만큼 늘여서 잡을 것 (적어도 LandScape 범위 밖에 위치해야 한다.)
	Vector3 TempRayCheckEdPos = RayStPosWithYZero + RayDirWithYZero * LandScapeComponent->GetMeshSize() * LandScapeComponent->GetRelativeScale() * 1000.f;
	TempRayCheckEdPos.y = 0.f;
	
	std::vector<Vector3> vecIntersects;

	// 아래 가로
	bool IntersectLineToLineResult = GetLineIntersect(LandScapeMin, Vector3(LandScapeMax.x, 0.f, LandScapeMin.z),
		RayStPosWithYZero, TempRayCheckEdPos, vecIntersects);
	// 왼쪽 세로
	IntersectLineToLineResult = GetLineIntersect(LandScapeMin, Vector3(LandScapeMin.x, 0.f, LandScapeMax.z),
		RayStPosWithYZero, TempRayCheckEdPos, vecIntersects);
	// 위쪽 가로
	IntersectLineToLineResult = GetLineIntersect(Vector3(LandScapeMin.x, 0.f, LandScapeMax.z), LandScapeMax,
		RayStPosWithYZero, TempRayCheckEdPos, vecIntersects);
	// 오른쪽 세로 
	IntersectLineToLineResult = GetLineIntersect(Vector3(LandScapeMax.x, 0.f, LandScapeMin.z), LandScapeMax,
		RayStPosWithYZero, TempRayCheckEdPos, vecIntersects);

	// 찾아낸 교차점이 LandScape 밖에 있는지 확인한다.
	auto iterSt = vecIntersects.begin();
	auto iterEd = vecIntersects.end();

	// 가끔 ex) MaxZ 가 128인데, 128.0001 이런 숫자가 나올때가 있다.
	float Offset = 0.01f;

	for (; iterSt != iterEd;)
	{
		if ((*iterSt).x < LandScapeMin.x - Offset || (*iterSt).z < LandScapeMin.z - Offset ||
			(*iterSt).x > LandScapeMax.x + Offset || (*iterSt).z > LandScapeMax.z + Offset)
		{
			iterSt = vecIntersects.erase(iterSt);
			continue;
		}
		++iterSt;
	}

	// 1) Ray  시작점이 LandScape 안에 있다면, 1개의 교차점이 나와야 하고, 그것이 바로 End
	// 2) Ray 시작점이 LandScape 밖에 있다면, 정상적인 경우, 교차점 2개가 다 나와야 한다.
	// 3) 그외의 경우 -> Ray가 LandScape 를 지나지 않는다는 것이다.
	if (vecIntersects.size() <= 0)
		return false;

	bool IsRayStInsideLandScape = false;

	if (RayStPosWithYZero.x >= LandScapeMin.x && RayStPosWithYZero.z >= LandScapeMin.z &&
		RayStPosWithYZero.x <= LandScapeMax.x && RayStPosWithYZero.z <= LandScapeMax.z)
	{
		// LandScape 안쪽에 Ray의 Start 위치가 있다는 것이다.
		IsRayStInsideLandScape = true;
		RayFinalCheckStPos = RayStPosWithYZero;
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

	// X 값이 더 작은 것을, St P 로 세팅할 것이다.
	// 즉, 더 왼쪽에 있는 녀석 (열이 더 왼쪽)
	// 이래야만, 아래의 BresenHam 알고리즘이 적용된다. (열이 더 낮은 녀석에서, 열이 더 큰 녀석으로 가는 것)
	if (RayFinalCheckEdPos.x < RayFinalCheckStPos.x)
	{
		Vector3 Temp = RayFinalCheckEdPos;
		RayFinalCheckEdPos = RayFinalCheckStPos;
		RayFinalCheckStPos = RayFinalCheckEdPos;
	}

	// >> 3. Bresenham 알고리즘을 이용해서, 해당 Ray가 지나가는 LandScape 격자 영역 목록을 뽑아낸다
	// St, Ed 지점에 대해서 LandScape 상의 2차원 좌표값을 구한다.
	// 예를 들어, CountX,Z 가 각각 129 개라면
	// 실제 사각형 개수는 각각 128, 128개이다.
	float LandScapeSizeX = (LandScapeMax.x - LandScapeMin.x) / (float)(LandScapeComponent->GetCountX() - 1);
	float LandScapeSizeZ = (LandScapeMax.z - LandScapeMin.z) / (float)(LandScapeComponent->GetCountZ() - 1);

	// X Idx : 열
	// Z Idx : 행
	int RayStartXIdx = (int)((RayFinalCheckStPos.x - LandScapeMin.x) / LandScapeSizeX);
	int RayStartZIdx = (int)((RayFinalCheckStPos.z - LandScapeMin.z) / LandScapeSizeZ);

	int RayEndXIdx = (int)((RayFinalCheckEdPos.x - LandScapeMin.x) / LandScapeSizeX);
	int RayEndZIdx = (int)((RayFinalCheckEdPos.z - LandScapeMin.z) / LandScapeSizeZ);

	// 예를 들어, CountX, CountZ 가 129, 129 라고 한다면
	// 실제 사각형 개수는 128 * 128 개이고
	// 각각의 Idx 범위는 0 ~ 127 이 되어야 한다.
	// 따라서 이에 맞게 제한을 줘야 한다.
	if (RayStartXIdx >= LandScapeComponent->GetCountX() - 1)
		RayStartXIdx = LandScapeComponent->GetCountX() - 2;

	if (RayEndXIdx >= LandScapeComponent->GetCountX() - 1)
		RayEndXIdx = LandScapeComponent->GetCountX() - 2;

	if (RayStartZIdx >= LandScapeComponent->GetCountZ() - 1)
		RayStartZIdx = LandScapeComponent->GetCountZ() - 2;

	if (RayEndZIdx >= LandScapeComponent->GetCountZ() - 1)
		RayEndZIdx = LandScapeComponent->GetCountZ() - 2;

	// LandScape 4개의 변 각각에 대한 직선 방정식을 정의한다.
	std::vector<std::pair<int, int>> vecLandScapeRayGoingThroughIdx;
	vecLandScapeRayGoingThroughIdx.reserve((size_t)LandScapeComponent->GetCountX() * (size_t)LandScapeComponent->GetCountZ());

	// Z: 행
	// X : 열
	// (Z, X) 정보 반환
	// 1) RaySt -> Ray End 의 기울기가 증가하는 형태라면, 기존의 Bresenham 알고리즘을 그대로 사용해도 된다.
	// 2) 반면, 기울기가 감소하는 형태라면, 방향을 돌려서, Bresenham을 적용한 뒤, 추출해야 한다.
	// - 즉, 마치 왼쪽 상단에서, 오른쪽 하단 측으로 증가하는 듯한 (왼쪽에서 오른쪽을 바라본 형태로 적용)
	if (RayStartZIdx <= RayEndZIdx)
	{
		// 시작 행, 시작 열, 끝 행, 끝 열
		// Bresenham(int stR, int stC, int edR, int edC, std::vector<std::pair<int, int>>& vecIdxP)
		Bresenham(RayStartZIdx, RayStartXIdx, RayEndZIdx, RayEndXIdx, vecLandScapeRayGoingThroughIdx);
	}
	else
	{
		// 예시 1
		/* (Before)
		2,0 / 2,1 / 2,2 / 2,3 / 2,4
		1,0 / 1,1 / 1,2 / 1,3 / 1,4
		0,0 / 0,1 / 0,2 / 0,3 / 0,4
		*/

		/* (After)
		0,0 / 1,0 / 2.0 / 3,0 / 4,0
		0,1 / 1,1 / 2,1 / 3,1 / 4,1
		0,2 / 1,2 / 2,2 / 3,2 / 4,3
		*/

		// 예시 2
		/* (Before)
		4,0 / 4,1 / 4,2
		3,0 / 3,1 / 3.2
		2,0 / 2,1 / 2,2
		1,0 / 1,1 / 1,2
		0,0 / 0,1 / 0,2
		*/

		/* (After)
		0,0 / 1,0 / 2.0
		0,1 / 1,1 / 2,1
		0,2 / 1,2 / 2,2
		0,3 / 1,3 / 2,3
		0,4 / 1,4 / 2,4
		*/

		// (Before -> After)
		// 시계방향으로 90도 돌리는 느낌 (물론, 90도 돌리는 것은 아니다, 결과적으로 행과 열의 개수가 달라질 수 있기 때문)
		// (bR, bC) , 총 R행, 총 C열
		// 1) 열 -> 행
		// 2) (총 행 - 1) - 현재 행 -> 열
		int ChangedStRowIdx = RayStartXIdx;
		int ChangedStColIdx = (LandScapeComponent->GetCountZ() - 1) - RayStartZIdx;

		int ChangedEdRowIdx = RayEndXIdx;
		int ChangedEdColIdx = (LandScapeComponent->GetCountZ() - 1) - RayEndZIdx;

		// 시작 열,행 - 끝 열,행
		Bresenham(ChangedStRowIdx, ChangedStColIdx, ChangedEdRowIdx, ChangedEdColIdx, vecLandScapeRayGoingThroughIdx);

		// 그리고 얻어낸 (행,열) 정보는 또 다시 바꿔줘야 한다. (After -> Before)
		// 1) 행 -> 열
		// 2) (총 열 - 1) - 현재 열 -> 행
		for (int i = 0; i < vecLandScapeRayGoingThroughIdx.size(); ++i)
		{
			// 행 , 열 (Z, X)
			auto [ZIdx, XIdx] = vecLandScapeRayGoingThroughIdx[i];
			vecLandScapeRayGoingThroughIdx[i] = std::make_pair(LandScapeComponent->GetCountX() - 1- XIdx,ZIdx);
		}
	}


	// 혹시나 지나가는 Idx 정보가 존재하지 않는다면 Return -> 여기에 걸리면 안되는 것 아닌가 ?
	if (vecLandScapeRayGoingThroughIdx.size() <= 0)
		return false;

	// >> 4. 해당 격자 안에 있는 삼각형 목록을 뽑아낸다.
	// vecRayGoingThroughIdx 안에 들어있는 Idx 정보는, 왼쪽 하단 -> 오른쪽 상단 방향으로 증가하는
	// LandScape 사각형 단위의 Idx 정보이다.

	// vecTriangleRayGoingThroughIdx 의 경우, 왼쪽 상단 -> 오른쪽 하단 방향으로 내려오는 삼각형 판별 기준 Idx
	// 즉, LandScape 의 사각형 격자 Idx 단위와 , 삼각형 Idx 단위는 다르기 때문에, 중간에 조정 과정을 거쳐야 한다.
	// 삼각형 - Ray 까지 거리 , 삼각형 Idx . 에 대한 정보들을 담을 것이다.
	// 차후, 삼각형 - Ray 까지 거리  를 기준으로 오름차순 정렬해주기 위해서, "삼각형 - Ray 까지 거리" 를 먼저 담는다.
	std::vector<float> vecTriangleRayDist;

	// Z: 행 Idx, X : 열 Idx
	for (const auto &[ZIdx, XIdx] : vecLandScapeRayGoingThroughIdx)
	{ 
		// 특정 격자안의 2개 삼각형 중에서 충돌하는 삼각형이 있는지 검사한다.
		auto IntersectResult = LandScapeComponent->CheckRayIntersectsTriangleInLandScape(XIdx, ZIdx,
			ray.Pos, ray.Dir);

		if (IntersectResult.has_value())
		{
			float DistToTriangle = IntersectResult.value();
			vecTriangleRayDist.push_back(DistToTriangle);
		}
	}

	// 4. 해당 삼각형 목록 중에서 Ray 와 가장 가까이 위치한 삼각형 정보를 뽑아낸다.
	// 혹시나 충돌하는 삼각형 정보가 없다면 Break -> 이것은 가능성 있다. Bounding Volume 과만 부딪힐 뿐
	// 실제 Traingle 들과는 부딪히지 않을 수도 있기 때문이다.
	if (vecTriangleRayDist.size() <= 0)
		return false;

	std::sort(vecTriangleRayDist.begin(), vecTriangleRayDist.end());

	float DistToTriangle = vecTriangleRayDist[0];

	PickedWorldPos = ray.Pos + ray.Dir * DistToTriangle;

	return true;
}

// https://devvcxxx.tistory.com/38
bool CPickingLogic::GetLineIntersect(const Vector3& StartPoint, const Vector3& EndPoint,
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

bool CPickingLogic::CheckRayTriangleIntersect(
	const Vector3& rayOrig, const Vector3& rayDir,
	const Vector3& v0, const Vector3& v1, const Vector3& v2,
	float& IntersectDist, Vector3& IntersectPoint)
{
	// 1단계. 먼저, 평면 ~ 직선. 이 거의 평행한지 여부를 살핀다.
	// 평면의 Normal 값을 계산한다.
	// compute plane's normal
	Vector3 Edge1 = v1 - v0;
	Vector3 Edge2 = v2 - v0;

	// Normalize 할 필요는 없다.
	Vector3 NormalV = Edge1.Cross(Edge2);  //N 

	// 거의 0
	float kEpsilon = 0.00001;

	// 평면의 Normal Vector 와, Ray 의 Dir의 Dot 결과가
	// 거의 0이라면, 둘은 거의 수직이라는 것
	// 반대로 말하면, 평면과 Ray 는 거의 평행하다는 것
	float NormalVDotRayDir = NormalV.Dot(rayDir);
	
	if (abs(NormalVDotRayDir) < kEpsilon)  //almost 0 
		return false;  //they are parallel so they don't intersect ! 

	// 2단계 : 평면과의 교차점 구하기 
	// Ax + By + Cz + D = 0 이라는 평면의 방정식에서
	// D 를 구하기 
	// Normal Vector 의 원소값 * 평면 위 임의 한점과의 원소값
	float PlaneDist = NormalV.Dot(v0) * -1;

	// Ray 에서 평면까지의 거리를 구한다.
	// Ax + By + Cz + D 에서
	// x,y,z 에 Ray 방정식 (StartPos + IntersectDist * RayDir) 을 대입
	// 이를 통해, IntersectDist (미지수) 값을 도출해낼 수 있다.
	// compute t (equation 3)
	IntersectDist = -1 * (NormalV.Dot(rayOrig) + PlaneDist) / NormalVDotRayDir;

	// 3단계 : Ray 의 시작점이 Triangle 보다 앞에 있는지 확인
	// 즉, Ray Dir 방향 반대 쪽에 Triangle이 위치한다면
	// IntersectDist 는 음수가 나올 것이다.
	if (IntersectDist < 0) 
		return false; 

	// 4단계 : 교차점 좌표 구하기 
	IntersectPoint = rayOrig + Vector3(rayDir.x * IntersectDist, rayDir.y * IntersectDist, rayDir.z * IntersectDist);
	
	// 5단계 : 교차점이 삼각형 내에 존재하는지 확인
	Vector3 CkEdge = v1 - v0;
	Vector3 CkPointEdge = IntersectPoint - v0;
	Vector3 CkCrossResult = CkEdge.Cross(CkPointEdge);

	if (NormalV.Dot(CkCrossResult) < 0)
		return false;

	CkEdge = v2 - v1;
	CkPointEdge = IntersectPoint - v1;
	CkCrossResult = CkEdge.Cross(CkPointEdge);

	if (NormalV.Dot(CkCrossResult) < 0)
		return false;

	CkEdge = v0 - v2;
	CkPointEdge = IntersectPoint - v2;
	CkCrossResult = CkEdge.Cross(CkPointEdge);

	if (NormalV.Dot(CkCrossResult) < 0)
		return false;

	return true;  //this ray hits the triangle 
}

/*


*/