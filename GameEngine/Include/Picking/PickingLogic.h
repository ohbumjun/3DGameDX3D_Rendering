#pragma once
#include "../GameInfo.h"

class CPickingLogic
{
public:
	static bool Picking(class CGameObject*& result);
	static void Bresenham(int stR, int stC, int edR, int edC, std::vector<std::pair<int, int>>& vecP);
	static bool DDTPicking(class CGameObject* LandScapeObject, class CGameObject* Player, Vector3& PickedWorldPos);
	// 점 2개 , Ray Start 지점, Ray 방향 정보를 준다
	// 그러면, 점 2개를 이은 직선 ~ Ray 직선 과의 교차점을 구해준다.
	// 단, 모든 정보들은 같은 좌표계 상에 위치해야 한다.
	static bool GetIntersectPoints(const Vector3& StartPoint, const Vector3& EndPoint,
		const Vector3& RayStartPos, const Vector3& RayDir, std::vector<Vector3>& vecIntersects);
};

