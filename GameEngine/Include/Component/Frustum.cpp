
#include "Frustum.h"

CFrustum::CFrustum()
{
	m_Pos[0] = Vector3(-1.f, 1.f, 0.f);
	m_Pos[1] = Vector3(1.f, 1.f, 0.f);
	m_Pos[2] = Vector3(-1.f, -1.f, 0.f);
	m_Pos[3] = Vector3(1.f, -1.f, 0.f);

	m_Pos[4] = Vector3(-1.f, 1.f, 1.f);
	m_Pos[5] = Vector3(1.f, 1.f, 1.f);
	m_Pos[6] = Vector3(-1.f, -1.f, 1.f);
	m_Pos[7] = Vector3(1.f, -1.f, 1.f);
}

CFrustum::~CFrustum()
{
}

void CFrustum::Update(Matrix matVP)
{
	// 월드공간에서의 8개 위치, 즉, 꼭짓점 좌표들을 구한다.
	Vector3	Pos[8];

	// 역행렬을 구한다.
	matVP.Inverse();

	// 우리의 의도는, 투영 공간에 있는 좌표점들을, view * proj 행렬의 역행렬을 곱해서
	// World 공간으로 변환하려는 것이다.
	// 그렇다면 전제는 해당 좌표점들이 투영 공간에 위치해있어야 한다는 것인데
	// 처음 물음은, 아니. 애초에 해당 점들을 투영 공간으로 변환 시키는 과정이 없었는데 어떻게
	// 이러한 과정이 가능한 것인가 였다.
	// - 바로, 애초에 위 생성자에서 투영 공간 상 (즉, NDC 좌표상의 투영 공간 좌표) 로 만들어줬기 때문에
	// - 이런 과정이 가능한 것이다.
	// - (뭔가 가정이, 투영공간 변환 이후, 좌표들은 x,y 는 -1에서 1 사이
	// z 는 0 에서 1 사이)
	for (int i = 0; i < 8; ++i)
		Pos[i] = m_Pos[i].TransformCoord(matVP);

	// Left
	m_Plane[(int)Frustum_Plane_Dir::Left] = XMPlaneFromPoints(Pos[4].Convert(), Pos[0].Convert(),
		Pos[2].Convert());

	// Right
	m_Plane[(int)Frustum_Plane_Dir::Right] = XMPlaneFromPoints(Pos[1].Convert(), Pos[5].Convert(),
		Pos[7].Convert());

	// Top
	m_Plane[(int)Frustum_Plane_Dir::Top] = XMPlaneFromPoints(Pos[4].Convert(), Pos[5].Convert(),
		Pos[1].Convert());

	// Bottom
	m_Plane[(int)Frustum_Plane_Dir::Bottom] = XMPlaneFromPoints(Pos[2].Convert(), Pos[3].Convert(),
		Pos[7].Convert());

	// Near
	m_Plane[(int)Frustum_Plane_Dir::Near] = XMPlaneFromPoints(Pos[0].Convert(), Pos[1].Convert(),
		Pos[3].Convert());

	// Far
	m_Plane[(int)Frustum_Plane_Dir::Far] = XMPlaneFromPoints(Pos[5].Convert(), Pos[4].Convert(),
		Pos[6].Convert());
}

// https://m.blog.naver.com/PostView.naver?isHttpsRedirect=true&blogId=ya3344&logNo=221400519972
bool CFrustum::FrustumInPoint(const Vector3& Point)
{
	for (int i = 0; i < (int)Frustum_Plane_Dir::Max; ++i)
	{
		// m_Plane[i].Convert() : ax + by + cz + d = 0 으로 정의되는 평면이다.
		// 해당 평면과 Point.Convert() 라는 점 사이의 거리는
		// Point.Convert() 라는 점 x1, y1, z1 과 해당 평면 사이의 거리
		// ax1 + by1 + cz1 + d = k
		// 이것의 결과값을 가지고 판단해야 한다.
		// (참고 : d 는 방향성 부호이다. 평면에서 원점 방향으로의 벡터 방향이
		// Normal 벡터 방향과 같다면 + , 그게 아니라면 - 이다)
		// k > 0 : 평면 뒤쪽 (평면 밖)
		// k = 0 : 평면 위
		// k < 0 : 평면 안쪽 (평면 안)
		// 현재 각각의 평면 Normal 벡터들은, Frustum 안쪽에서, 바깥쪽으로 향하는 평면들
		// 만약 해당 값들 중 하나라도 양수라는 것은, 해당 평면 밖에 위치한다는 것
		// 즉, Frustum 밖에 위치한다는 것이다.
		// 이 경우, Culling 이 되어, 화면에 보여지면 안되는 녀석이다.
		float	Dot = XMVectorGetX(XMPlaneDotCoord(m_Plane[i].Convert(), Point.Convert()));

		if (Dot > 0.f)
			return true;
	}

	return false;
}

bool CFrustum::FrustumInSphere(const SphereInfo& Sphere)
{
	// true 라는 것은, culling 이 되었다는 것
	// 즉, 화면에서 보이지 않는 녀석이라는 것 

	for (int i = 0; i < (int)Frustum_Plane_Dir::Max; ++i)
	{
		// 아래의 경우, 평면의 방정식에서 d 를 결정하는 원점이 어디인걸까 
		// World Space 상에서 계산을 진행 중이므로, World Space 상의 평면이라고 해도 되지 않을까 ?

		// XMPlaneDotCoord의 경우, point ~ plane 사이의 거리를 측정하는데 사용된다.
		
		// 평면과 Vector3 와의 Dot Product 결과를 Return
		// 어차피 x,y,z,w 값이 다 동일하므로 GetX 를 통해 하나의 값만 가져온다.
		
		// World 공간 상에서 평면 ~ 점. 사이의 거리를 측정할 것이다.
		// XMPlaneDotCoord 의 결과는 양수, 0, 음수
		// 0, 음수라면, SphereInfo 의 중심이 Frustum 안쪽에 있다는 의미이므로
		// 절두체 Culling의 대상이 아니다.
		// 조사할 대상은, 해당 결과가 양수일 때이다.
		
		// 평면의 법선 벡터는, 절두체 밖을 향하고 있다.
		
		// 어떤 물체의 BV 원이 절두체 밖에 위치하고 있다고 해보자.
		// 해당 BV 원의 중심 ~ 평면 사이의 거리가 반지름보다 작다면
		// BV 원의 일부분이, 평면 밖에서, 절두체 안쪽으로 걸쳐져 있다는 것
		// 이 경우는 절두체 안에 들어오는 것이므로 그려내야 한다.
		// 만약, BV 원의 중심이 절두체 안에 존재한다면 ?
		float	Dot = XMVectorGetX(XMPlaneDotCoord(m_Plane[i].Convert(), Sphere.Center.Convert()));

		if (Dot > Sphere.Radius)
			return true;
	}

	return false;
}
