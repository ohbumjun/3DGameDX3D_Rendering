#pragma once

#include "../GameInfo.h"

enum class Frustum_Plane_Dir
{
	Left,
	Right,
	Top,
	Bottom,
	Near,
	Far,
	Max
};

class CFrustum
{
	friend class CCameraComponent;

private:
	CFrustum();
	~CFrustum();

private:
	Vector3	m_Pos[8];
	Vector4	m_Plane[(int)Frustum_Plane_Dir::Max];

public:
	void Update(Matrix matVP);
	bool FrustumInPoint(const Vector3& Point);
	bool FrustumInSphere(const SphereInfo& Sphere);
};

