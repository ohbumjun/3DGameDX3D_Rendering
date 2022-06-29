#include "NavigationMesh.h"
#include "../PathManager.h"

CNavigationMesh::CNavigationMesh()
{
	m_Grid = false;
	m_LineRectCount = 0;
	m_SectionX = 1;
	m_SectionZ = 1;
	m_Section = new NavSection[m_SectionX * m_SectionZ];

	m_HeightGap = 0.5f;
	m_UseCellListSize = 0;
	m_UseCellList = nullptr;

	m_OpenList = nullptr;
	m_LastPath = -1;
	m_StartCell = nullptr;

	m_ArriveDist = 0.f;
}

CNavigationMesh::~CNavigationMesh()
{
	SAFE_DELETE_ARRAY(m_OpenList);
	SAFE_DELETE_ARRAY(m_UseCellList);
	SAFE_DELETE_ARRAY(m_Section);
	auto	iter = m_vecCell.begin();
	auto	iterEnd = m_vecCell.end();

	for (; iter != iterEnd; ++iter)
	{
		SAFE_DELETE((*iter));
	}
}

void CNavigationMesh::SetCellSize(float X, float Z)
{
	m_CellSize.x = X;
	m_CellSize.z = Z;
}

void CNavigationMesh::ComputeArriveDist(float X, float Y)
{
	m_ArriveDist = (X + Y) * 0.05f;
}

float CNavigationMesh::GetArriveDist() const
{
	return m_ArriveDist;
}

Vector3 CNavigationMesh::GetMin() const
{
	return m_Min;
}

Vector3 CNavigationMesh::GetMax() const
{
	return m_Max;
}

Vector3 CNavigationMesh::GetOffset() const
{
	return m_Offset;
}

list<Vector3> CNavigationMesh::GetPathList() const
{
	return m_PathList;
}

string CNavigationMesh::GetName() const
{
	return m_Name;
}

void CNavigationMesh::SetName(const string& Name)
{
	m_Name = Name;
}

void CNavigationMesh::SetOffset(const Vector3& Offset)
{
	m_Offset = Offset;
}

void CNavigationMesh::SetOffsetScale(const Vector3& OffsetScale)
{
	m_OffsetScale = OffsetScale;
}

bool CNavigationMesh::Init()
{
	m_Min.x = 10000000.f;
	m_Min.y = 10000000.f;
	m_Min.z = 10000000.f;

	m_Max.x = -10000000.f;
	m_Max.y = -10000000.f;
	m_Max.z = -10000000.f;

	return true;
}

void CNavigationMesh::ClearCell()
{
	auto	iter = m_vecCell.begin();
	auto	iterEnd = m_vecCell.end();

	for (; iter != iterEnd; ++iter)
	{
		SAFE_DELETE((*iter));
	}
}

void CNavigationMesh::AddCell(const Vector3 Pos[3])
{
	NavigationCell*	Cell = new NavigationCell;

	for (int i = 0; i < 3; ++i)
	{
		Cell->Pos[i] = Pos[i];
	}

	Cell->Edge[0] = Pos[1] - Pos[0];
	Cell->EdgeCenter[0] = (Pos[1] + Pos[0]) / 2.f;

	Cell->Edge[1] = Pos[2] - Pos[1];
	Cell->EdgeCenter[1] = (Pos[2] + Pos[1]) / 2.f;

	Cell->Edge[2] = Pos[2] - Pos[0];
	Cell->EdgeCenter[2] = (Pos[2] + Pos[0]) / 2.f;

	Cell->Center = (Pos[0] + Pos[1] + Pos[2]) / 3.f;

	Cell->Index = m_vecCell.size();

	for (int i = 0; i < 3; ++i)
	{
		if (m_Min.x > Pos[i].x)
			m_Min.x = Pos[i].x;
		if (m_Min.y > Pos[i].y)
			m_Min.y = Pos[i].y;
		if (m_Min.z > Pos[i].z)
			m_Min.z = Pos[i].z;

		if (m_Max.x < Pos[i].x)
			m_Max.x = Pos[i].x;
		if (m_Max.y < Pos[i].y)
			m_Max.y = Pos[i].y;
		if (m_Max.z < Pos[i].z)
			m_Max.z = Pos[i].z;
	}

	Vector3	Dir1 = Cell->Edge[0];
	Vector3	Dir2 = Cell->Edge[2];

	Dir1.Normalize();
	Dir2.Normalize();

	Vector3	FaceNormal = Dir1.Cross(Dir2);
	FaceNormal.Normalize();

	Vector3	View = Vector3::Axis[AXIS_Y];

	float	fAngle = View.Angle(FaceNormal);

	if (fAngle >= 50.f)
	{
		float	fMaxY = Pos[0].y;
		float	fMinY = Pos[0].y;

		fMaxY = fMaxY < Pos[1].y ? Pos[1].y : fMaxY;
		fMaxY = fMaxY < Pos[2].y ? Pos[2].y : fMaxY;

		fMinY = fMinY > Pos[1].y ? Pos[1].y : fMinY;
		fMinY = fMinY > Pos[2].y ? Pos[2].y : fMinY;

		float	fHeight = fMaxY - fMinY;

		if (m_HeightGap <= fHeight)
			Cell->Enable = false;
	}

	m_vecCell.push_back(Cell);
}

void CNavigationMesh::AddAdj(int CellIdx, int AdjIdx)
{
	if (CellIdx < 0 || CellIdx >= m_vecCell.size())
		return;

	AdjInfo	Info = {};
	Info.Index = AdjIdx;
	Info.EdgeIndex = 0;
	m_vecCell[CellIdx]->vecAdj.push_back(Info);
}

// 그리드 형태의 삼각형 인접정보를 만들어낸다.
void CNavigationMesh::CreateGridMapAdj(int LineRectCount)
{
	m_Grid = true;
	m_LineRectCount = LineRectCount;

	size_t Size = m_vecCell.size();
	AdjInfo	Info = {};

	for (size_t i = 0; i < Size; i += 2)
	{
		// 우상단 삼각형 인접정보 구성.
		// 위쪽 사각형, 오른쪽 사각형, 자기자신 사각형의 왼쪽 하단 삼각형이
		// 인접정보 후보가 된다.
		// 위쪽 사각형의 좌하단 삼각형 인덱스를 구한다.
		int	idx = i - LineRectCount * 2 + 1;
		if (idx >= 0 && idx < m_vecCell.size())
		{
			Info.Index = idx;
			Info.EdgeIndex = 0;
			m_vecCell[i]->vecAdj.push_back(Info);
		}

		// 자기자신 사각형의 왼쪽 하단 삼각형 인덱스를 구한다.
		idx = i + 1;
		if (idx >= 0 && idx < m_vecCell.size())
		{
			Info.Index = idx;
			Info.EdgeIndex = 2;
			m_vecCell[i]->vecAdj.push_back(Info);
		}

		// 오른쪽 사각형의 왼쪽 하단 삼각형 인덱스를 구한다.
		if ((i / 2) % LineRectCount != LineRectCount - 1)
		{
			idx = i + 3;
			Info.Index = idx;
			Info.EdgeIndex = 1;
			m_vecCell[i]->vecAdj.push_back(Info);
		}

		// 좌하단 삼각형 인접정보 구성.
		// 아래쪽 사각형, 왼쪽 사각형, 자기자신 사각형의 오른쪽 상단 삼각형이
		// 인접정보 후보가 된다.
		// 자기자신 사각형의 우상단 삼각형 인덱스를 구한다.
		Info.Index = i;
		Info.EdgeIndex = 0;
		m_vecCell[i + 1]->vecAdj.push_back(Info);

		// 왼쪽 사각형의 우상단 삼각형 인덱스를 구한다.
		if ((i / 2) % LineRectCount != 0)
		{
			Info.Index = i - 3;
			Info.EdgeIndex = 2;
			m_vecCell[i + 1]->vecAdj.push_back(Info);
		}

		idx = i + (LineRectCount * 2);
		if (idx < m_vecCell.size())
		{
			Info.Index = idx;
			Info.EdgeIndex = 1;
			m_vecCell[i + 1]->vecAdj.push_back(Info);
		}
	}

	CreateSection();
}

// 그리드 형태가 아닌 불규칙한 형태의 삼각형 인접정보를 만들어낸다.
void CNavigationMesh::CreateAdj()
{
	size_t	Size = m_vecCell.size();

	FILE* File = NULL;

	fopen_s(&File, "NaviAdj.txt", "wt");

	Vector3	Origin[2][3];
	Vector3	EdgeDir[2][3];
	float	Length[2][3];

	for (size_t i = 0; i < Size; ++i)
	{
		Vector3	Src[3];
		Vector3	SrcOrigin[3];
		float	SrcLength[3] = {};
		for (int k = 0; k < 3; ++k)
		{
			EdgeDir[0][k] = m_vecCell[i]->Edge[k];

			Length[0][k] = EdgeDir[0][k].Length();

			EdgeDir[0][k].Normalize();
		}

		Origin[0][0] = m_vecCell[i]->Pos[0];
		Origin[0][1] = m_vecCell[i]->Pos[1];
		Origin[0][2] = m_vecCell[i]->Pos[2];

		for (size_t j = i + 1; j < Size; ++j)
		{
			// 삼각형의 3개의 엣지들을 서로 내적하여 평행한지 비교한다.
			Vector3	Dest[3];
			float DestLength[3];
			Vector3	DestOrigin[3];
			int	PosIdx[3][2] = { 1, 0, 2, 1, 2, 0 };
			bool	Insert = false;

			Origin[1][0] = m_vecCell[j]->Pos[0];
			Origin[1][1] = m_vecCell[j]->Pos[1];
			Origin[1][2] = m_vecCell[j]->Pos[2];

			for (int k = 0; k < 3; ++k)
			{
				EdgeDir[1][k] = m_vecCell[j]->Edge[k];

				Length[1][k] = EdgeDir[1][k].Length();

				EdgeDir[1][k].Normalize();
			}

			for (int k = 0; k < 3; ++k)
			{
				if (Insert)
					break;

				for (int l = 0; l < 3; ++l)
				{
					// A삼각형과 B삼각형의 엣지를 구성하는 두 정점이 같을 경우
					// 이 삼각형은 무조건 붙어있는 삼각형이다.
					if ((Origin[0][PosIdx[k][0]] == Origin[1][PosIdx[l][0]] ||
						Origin[0][PosIdx[k][0]] == Origin[1][PosIdx[l][1]]) &&
						(Origin[0][PosIdx[k][1]] == Origin[1][PosIdx[l][0]] ||
							Origin[0][PosIdx[k][1]] == Origin[1][PosIdx[l][1]]))
					{
						AdjInfo	Info = {};
						Info.Index = j;
						Info.EdgeIndex = k;
						m_vecCell[i]->vecAdj.push_back(Info);

						Info.Index = i;
						Info.EdgeIndex = l;
						m_vecCell[j]->vecAdj.push_back(Info);
						Insert = true;
						break;
					}

					// 두 엣지에 대해서 상대 엣지를 구성하는 2개의 점이 대상 엣지
					// 위에 둘다 존재하는지를 판단한다. 둘다 존재한다면
					// 무조건 붙어있는 것이다.
					// (B1 - A1, B2 - A1), (B1 - A2, B2 - A2)
					// 내적을 통해서 빼준 벡터의 크기가 그대로 나온다면
					// 직선상에 존재하는 것이다. 단, 둘다 크기가 나올 경우는
					// 무조건 존재하는 것이고 둘중 하나만 나온다면 크기가 0이라면
					// 하나의 점만 직선위에 존재하고 두 점 위치가 같다는 것이므로
					// 해당 삼각형은 인접 삼각형이 아니다.
					// B1 - A1 처리
					if (CheckOnEdge(i, j, Origin[1][PosIdx[l][0]],
						Origin[0][PosIdx[k][0]], EdgeDir[0][k], Length[0][k], k, l))
					{
						Insert = true;
						break;
					}

					// B2 - A1 처리
					else if (CheckOnEdge(i, j, Origin[1][PosIdx[l][1]],
						Origin[0][PosIdx[k][0]], EdgeDir[0][k], Length[0][k], k, l))
					{
						Insert = true;
						break;
					}

					// B1 - A2 처리
					else if (CheckOnEdge(i, j, Origin[1][PosIdx[l][0]],
						Origin[0][PosIdx[k][1]], EdgeDir[0][k] * -1.f, Length[0][k], k, l))
					{
						Insert = true;
						break;
					}

					// B2 - A2 처리
					else if (CheckOnEdge(i, j, Origin[1][PosIdx[l][1]],
						Origin[0][PosIdx[k][1]], EdgeDir[0][k] * -1.f, Length[0][k], k, l))
					{
						Insert = true;
						break;
					}
				}
			}
		}
	}

	fclose(File);

	CreateSection();
}

bool CNavigationMesh::CheckOnEdge(int Src, int Dest,
	const Vector3& Origin1, const Vector3& Origin2,
	const Vector3& Edge, float EdgeLength, int Edge1, int Edge2)
{
	if ((int)(Origin2.x * 100) == (int)(Origin1.x * 100) && (int)(Origin2.y * 100) == (int)(Origin1.y * 100) &&
		(int)(Origin2.z * 100) == (int)(Origin1.z * 100))
		return false;

	Vector3	Result = Origin1 - Origin2;

	float	fDist = Result.Length();

	// 위에서 구해준 거리를 이용해서 원점으로부터 엣지 방향으로 지정된 거리만큼 이동된 위치에 대상 점이 있는지를
	// 판단한다.
	Result = Origin2 + Edge * fDist;

	if ((int)(Result.x * 100) == (int)(Origin1.x * 100) && (int)(Result.y * 100) == (int)(Origin1.y * 100) &&
		(int)(Result.z * 100) == (int)(Origin1.z * 100))
	{
		Result -= Origin2;
		if (Result.Length() < EdgeLength)
		{
			AdjInfo	Info = {};
			Info.Index = Dest;
			Info.EdgeIndex = Edge1;
			m_vecCell[Src]->vecAdj.push_back(Info);

			Info.Index = Src;
			Info.EdgeIndex = Edge2;
			m_vecCell[Dest]->vecAdj.push_back(Info);
			return true;
		}
	}

	return false;
}

float CNavigationMesh::ccw(const Vector2& v1, const Vector2& v2)
{
	return v1.x * v2.y - v1.y * v2.x;
}

float CNavigationMesh::ccw(const Vector2& v1, const Vector2& v2, const Vector2& v3)
{
	return ccw(v2 - v3, v3 - v1);
}

bool CNavigationMesh::CheckPathDir(const Vector3& Src1,
	const Vector3& Src2, const Vector3& Dest1,
	const Vector3& Dest2, Vector3& vIntersect)
{
	Vector2	v1 = Vector2(Src1.x, Src1.z);
	Vector2	v2 = Vector2(Src2.x, Src2.z);
	Vector2	v3 = Vector2(Dest1.x, Dest1.z);
	Vector2	v4 = Vector2(Dest2.x, Dest2.z);

	float	ab = ccw(v1, v2, v3) * ccw(v1, v2, v4);
	float	cd = ccw(v3, v4, v1) * ccw(v3, v4, v2);

	if (ab == 0 && cd == 0)
	{
		if (v2.x < v1.x && v2.y < v1.y)
			swap(v1, v2);

		if (v4.x < v3.x && v4.y < v3.y)
			swap(v3, v4);

		return !((v2.x < v3.x&& v2.y < v3.y) || (v4.x < v1.x&& v4.y < v1.y));
	}

	return ab <= 0 && cd <= 0;
	/*double t;
	double s;
	double under = (Dest2.z - Dest1.z)*(Src2.x - Src1.x) - (Dest2.x - Dest1.x)*(Src2.z - Src1.z);
	if (under == 0)
	return false;

	double _t = (Dest2.x - Dest1.x)*(Src1.z - Dest1.z) - (Dest2.z - Dest1.z)*(Src1.x - Dest1.x);
	double _s = (Src2.x - Src1.x)*(Src1.z - Dest1.z) - (Src2.z - Src1.z)*(Src1.x - Dest1.x);

	t = _t / under;
	s = _s / under;

	if (t<0.0 || t>1.0 || s<0.0 || s>1.0)
	return false;

	if (_t == 0 && _s == 0)
	return false;

	vIntersect.x = Src1.x + t * (double)(Src2.x - Src1.x);
	vIntersect.z = Src1.z + t * (double)(Src2.z - Src1.z);

	return true;*/
}

void CNavigationMesh::FindPath(Vector3 Start,
	const Vector3& End)
{
	NavigationCell* StartCell = FindCell(Start);
	NavigationCell* EndCell = FindCell(End);

	if (!StartCell || !EndCell)
		return;

	else if (!StartCell->Enable)
		return;

	else if (!EndCell->Enable)
		return;

	if (StartCell == EndCell)
	{
		m_PathList.clear();
		m_PathList.push_back(End);
		return;
	}

	if (!m_UseCellList)
	{
		m_UseCellList = new NavigationCell*[m_vecCell.size()];
		m_OpenList = new NavigationCell*[m_vecCell.size()];

		memset(m_OpenList, 0, sizeof(NavigationCell*) * m_vecCell.size());
	}

	for (int i = 0; i < m_UseCellListSize; ++i)
	{
		m_UseCellList[i]->Type = NCLT_NONE;
		m_UseCellList[i]->ParentIdx = -1;
		m_UseCellList[i]->G = -1.f;
		m_UseCellList[i]->H = -1.f;
		m_UseCellList[i]->Total = -1.f;
	}

	m_OpenListSize = 0;
	m_UseCellListSize = 0;

	/*if (CheckPathAll(Start, End))
	{
	m_PathList.clear();
	m_PathList.push_back(End);
	m_Start = nullptr;
	return;
	}*/

	//else if(m_LastPath != -1)
	//{
	//	// 원래의 시작노드를 저장한다.
	//	m_Start = StartCell;
	//	m_Start = Start;

	//	StartCell->Type = NCLT_CLOSE;
	//	StartCell->G = 0.f;
	//	Vector3	Line = Start - End;
	//	StartCell->H = Line.x * Line.x +
	//		Line.z * Line.z;

	//	StartCell->H = sqrtf(StartCell->H);

	//	//StartCell->H = Start.Distance(End);
	//	StartCell->Total = StartCell->H;

	//	m_UseCellList[m_UseCellListSize] = StartCell;
	//	++m_UseCellListSize;

	//	// 브레젠험 알고리즘으로 구해준 장애물 이전의 인덱스를
	//	// 시작지점으로 하여 길을 탐색한다.
	//	Start = m_vecCell[m_LastPath]->Center;
	//	StartCell = m_vecCell[m_LastPath];
	//}

	//else
	//{
	//	m_Start = nullptr;
	//}

	while (!m_FindStack.empty())
	{
		m_FindStack.pop();
	}

	// 시작노드를 열린목록에 넣어준다.
	StartCell->Type = NCLT_OPEN;
	StartCell->G = 0.f;
	Vector3	Line = Start - End;
	StartCell->H = Line.x * Line.x +
		Line.z * Line.z;

	StartCell->H = sqrtf(StartCell->H);

	//StartCell->H = Start.Distance(End);
	StartCell->Total = StartCell->H;

	m_OpenList[m_OpenListSize] = StartCell;
	++m_OpenListSize;

	m_UseCellList[m_UseCellListSize] = StartCell;
	++m_UseCellListSize;

	m_UseCellList[m_UseCellListSize] = EndCell;
	++m_UseCellListSize;

	m_FindEnd = false;

	NavigationCell*	Cell = nullptr;

	while (m_OpenListSize != 0 && !m_FindEnd)
	{
		// 열린목록에서 셀을 얻어온다.
		--m_OpenListSize;
		Cell = m_OpenList[m_OpenListSize];


		// 얻어온 셀을 닫힌목록으로 만들어준다.
		Cell->Type = NCLT_CLOSE;

		AddOpenList(Cell, EndCell, Start, End);

		// 열린목록을 정렬한다.
		if (m_OpenListSize >= 2)
		{
			qsort(m_OpenList, m_OpenListSize,
				sizeof(NavigationCell*), CNavigationMesh::OpenListQSort);
		}
	}
}

float CNavigationMesh::GetY(const Vector3& Pos)
{
	if (!m_Grid)
	{
		int	iSectionIndex = GetSectionIndex(Pos);

		if (0 <= iSectionIndex && iSectionIndex < m_SectionX * m_SectionZ)
		{
			Vector3	Origin = Pos;
			Origin.y = m_Max.y;
			Vector3	Dir = Vector3(0.f, -1.f, 0.f);

			for (int i = 0; i < m_Section[iSectionIndex].List.Size; ++i)
			{
				Vector3	vIntersect;
				float	Dist = 0.f;
				NavigationCell*	Cell = m_Section[iSectionIndex].List.CellArray[i];

				if (RayIntersectTriangle(Origin, Dir, Cell->Pos[0],
					Cell->Pos[1],
					Cell->Pos[2],
					Dist, vIntersect))
					return vIntersect.y;
			}
		}

		return m_Min.y;
	}

	Vector3	CellSize = m_CellSize;
	Vector3	ConvertPos = Pos - m_Min;

	// 가로, 세로를 1로 만들어준다.
	ConvertPos.x /= CellSize.x;
	ConvertPos.z /= CellSize.z;

	// 사각형 인덱스를 구한다.
	int	idxX = (int)ConvertPos.x;
	int	idxZ = m_LineRectCount - ((int)ConvertPos.z + 1);

	if (idxX < 0 || idxX >= m_LineRectCount)
		return m_Min.y;

	else if (idxZ < 0 || idxZ >= m_LineRectCount)
		return m_Min.y;

	int	idx = (idxZ * m_LineRectCount + idxX) * 2;

	/*if (!m_vecCell[idx]->Enable)
	return m_Min.y;*/

	// 구해준 사각형의 좌상단 점을 구한다.
	Vector3	LTPos = m_vecCell[idx]->Pos[0];
	LTPos.x /= CellSize.x;
	LTPos.z /= CellSize.z;

	float	X = ConvertPos.x - LTPos.x;
	float	Z = LTPos.z - ConvertPos.z;

	// 우상단 삼각형일 경우
	if (X >= Z)
	{
		// Cell을 구성하는 점의 Y값을 얻어온다.
		float	fY[3] = {};
		for (int i = 0; i < 3; ++i)
		{
			fY[i] = m_vecCell[idx]->Pos[i].y;
		}

		return fY[0] + (fY[1] - fY[0]) * X + (fY[2] - fY[1]) * Z;
	}

	// 좌하단 삼각형일 경우
	float	fY[3] = {};
	for (int i = 0; i < 3; ++i)
	{
		fY[i] = m_vecCell[idx + 1]->Pos[i].y;
	}

	return fY[0] + (fY[1] - fY[2]) * X + (fY[2] - fY[0]) * Z;
}

bool CNavigationMesh::CheckCell(const Vector3& Pos)
{
	int	CellIndex = GetCellIndex(Pos);

	if (CellIndex == -1)
		return false;

	float	fY = GetY(CellIndex, Pos);

	if (!m_vecCell[CellIndex]->Enable ||
		(Pos.y - 10.f > fY || fY > Pos.y + 10.f))
		return false;

	return true;
}

bool CNavigationMesh::GetCellEnable(const Vector3& Pos)
{
	int	CellIndex = GetCellIndex(Pos);

	if (CellIndex == -1)
		return false;

	return m_vecCell[CellIndex]->Enable;
}

float CNavigationMesh::GetY(int CellIndex, const Vector3& Pos)
{
	NavigationCell*	Cell = m_vecCell[CellIndex];

	if (!m_Grid)
	{
		int	iSectionIndex = GetSectionIndex(Pos);

		if (0 <= iSectionIndex && iSectionIndex < m_SectionX * m_SectionZ)
		{
			Vector3	Origin = Pos;
			Origin.y = m_Max.y;
			Vector3	Dir = Vector3(0.f, -1.f, 0.f);

			Vector3	Intersect;
			float	Dist = 0.f;

			if (RayIntersectTriangle(Origin, Dir, Cell->Pos[0],
				Cell->Pos[1],
				Cell->Pos[2],
				Dist, Intersect))
				return Intersect.y;
		}

		return m_Min.y;
	}

	Vector3	CellSize = m_CellSize;
	Vector3	ConvertPos = Pos - m_Min;

	// 가로, 세로를 1로 만들어준다.
	ConvertPos.x /= CellSize.x;
	ConvertPos.z /= CellSize.z;

	if (!Cell->Enable)
		return m_Min.y;

	// 구해준 사각형의 좌상단 점을 구한다.
	Vector3	LTPos = Cell->Pos[0];
	LTPos.x /= CellSize.x;
	LTPos.z /= CellSize.z;

	float	X = ConvertPos.x - LTPos.x;
	float	Z = LTPos.z - ConvertPos.z;

	// Cell을 구성하는 점의 Y값을 얻어온다.
	float	fY[3] = {};
	for (int i = 0; i < 3; ++i)
	{
		fY[i] = Cell->Pos[i].y;
	}

	return fY[0] + (fY[1] - fY[0]) * X + (fY[2] - fY[1]) * Z;
}

void CNavigationMesh::CreateSection()
{
	m_SectionX = 5;
	m_SectionZ = 5;

	Vector3	vLength = m_Max - m_Min;
	m_SectionSize = vLength / Vector3(m_SectionX, 1, m_SectionZ);

	SAFE_DELETE_ARRAY(m_Section);

	m_Section = new NavSection[m_SectionX * m_SectionZ];

	for (int i = 0; i < m_SectionZ; ++i)
	{
		for (int j = 0; j < m_SectionX; ++j)
		{
			int	idx = i * m_SectionX + j;
			m_Section[idx].Size = m_SectionSize;
			m_Section[idx].Min = m_Min + m_SectionSize * Vector3(j, 0, i);
			m_Section[idx].Max = m_Min + m_SectionSize * Vector3((j + 1), 1, (i + 1));
		}
	}

	// 최대 4개의 영역에 포함될 수 있다.
	// 어디 포함되어 있는지 판단해야 한다. 제발.
	for (size_t i = 0; i < m_vecCell.size(); ++i)
	{
		// 삼각형을 구성하는 3개의 점을 이용해서 인덱스를 구한다.
		int	iMinX = 100000, iMinZ = 100000;
		int	iMaxX = -100000, iMaxZ = -100000;

		for (int j = 0; j < 3; ++j)
		{
			Vector3	Pos = m_vecCell[i]->Pos[j] - m_Min;
			Pos /= m_SectionSize;

			int x, z;
			x = (int)Pos.x;
			z = (int)Pos.z;

			if (iMinX > x)
				iMinX = x;

			if (iMaxX < x)
				iMaxX = x;

			if (iMinZ > z)
				iMinZ = z;

			if (iMaxZ < z)
				iMaxZ = z;
		}

		iMinX = iMinX < 0 ? 0 : iMinX;
		iMinZ = iMinZ < 0 ? 0 : iMinZ;
		iMaxX = iMaxX >= m_SectionX ? m_SectionX - 1 : iMaxX;
		iMaxZ = iMaxZ >= m_SectionZ ? m_SectionZ - 1 : iMaxZ;

		if (iMinX < m_SectionX && iMaxX >= 0 &&
			iMinZ < m_SectionZ && iMaxZ >= 0)
		{
			for (int j = iMinZ; j <= iMaxZ; ++j)
			{
				for (int k = iMinX; k <= iMaxX; ++k)
				{
					int	idx = j * m_SectionX + k;

					m_Section[idx].Add(m_vecCell[i]);
				}
			}
		}
	}
}

void CNavigationMesh::AddSection()
{
	// 최대 4개의 영역에 포함될 수 있다.
	// 어디 포함되어 있는지 판단해야 한다. 제발.
	for (size_t i = 0; i < m_vecCell.size(); ++i)
	{
		// 삼각형을 구성하는 3개의 점을 이용해서 인덱스를 구한다.
		int	iMinX = 100000, iMinZ = 100000;
		int	iMaxX = -100000, iMaxZ = -100000;

		for (int j = 0; j < 3; ++j)
		{
			Vector3	Pos = m_vecCell[i]->Pos[j] - m_Min;
			Pos /= m_SectionSize;

			int x, z;
			x = (int)Pos.x;
			z = (int)Pos.z;

			if (iMinX > x)
				iMinX = x;

			if (iMaxX < x)
				iMaxX = x;

			if (iMinZ > z)
				iMinZ = z;

			if (iMaxZ < z)
				iMaxZ = z;
		}

		iMinX = iMinX < 0 ? 0 : iMinX;
		iMinZ = iMinZ < 0 ? 0 : iMinZ;
		iMaxX = iMaxX >= m_SectionX ? m_SectionX - 1 : iMaxX;
		iMaxZ = iMaxZ >= m_SectionZ ? m_SectionZ - 1 : iMaxZ;

		if (iMinX < m_SectionX && iMaxX >= 0 &&
			iMinZ < m_SectionZ && iMaxZ >= 0)
		{
			for (int j = iMinZ; j <= iMaxZ; ++j)
			{
				for (int k = iMinX; k <= iMaxX; ++k)
				{
					int	idx = j * m_SectionX + k;

					m_Section[idx].Add(m_vecCell[i]);
				}
			}
		}
	}
}

bool CNavigationMesh::RayIntersectTriangle(Ray& tRay)
{
	for (int i = 0; i < m_SectionZ; ++i)
	{
		for (int j = 0; j < m_SectionX; ++j)
		{
			int	idx = i * m_SectionX + j;

			for (int i = 0; i < m_Section[idx].List.Size; ++i)
			{
				float	fDist = 0.f;
				NavigationCell*	Cell = m_Section[idx].List.CellArray[i];

				if (RayIntersectTriangle(tRay.Pos, tRay.Dir, Cell->Pos[0],
					Cell->Pos[1],
					Cell->Pos[2],
					fDist, tRay.ImpactPoint))
					return true;
			}
		}
	}

	return false;
}

void CNavigationMesh::AddOpenList(NavigationCell* Cell,
	NavigationCell* EndCell, Vector3 Start,
	const Vector3& End)
{
	size_t	iAdjSize = Cell->vecAdj.size();
	for (size_t i = 0; i < iAdjSize; ++i)
	{
		if (Cell->vecAdj[i].Index == -1)
			continue;

		NavigationCell*	Adj = m_vecCell[Cell->vecAdj[i].Index];

		if (!Adj->Enable)
			continue;

		else if (Adj->Type == NCLT_CLOSE)
			continue;

		// 도착 셀을 찾았을 경우
		if (Adj == EndCell)
		{
			// 부모의 인덱스를 얻어온다.
			int	iParent = Cell->Index;

			while (iParent != -1)
			{
				m_FindStack.push(iParent);
				iParent = m_vecCell[iParent]->ParentIdx;
			}

			// 시작노드는 제거해준다.
			m_FindStack.pop();

			// 스택으로 만들어진 경로를 원래대로 되돌린다.
			vector<int>	vecPathIndex;

			while (!m_FindStack.empty())
			{
				int	idx = m_FindStack.top();
				m_FindStack.pop();
				vecPathIndex.push_back(idx);
			}

			m_PathList.clear();

			vector<Vector3>	vecCenter;
			for (int j = 0; j < (int)vecPathIndex.size() - 1; ++j)
			{
				int	EdgeIndex = -1;

				Cell = m_vecCell[vecPathIndex[j]];

				for (size_t k = 0; k < Cell->vecAdj.size(); ++k)
				{
					if (Cell->vecAdj[k].Index == vecPathIndex[j + 1])
					{
						EdgeIndex = Cell->vecAdj[k].EdgeIndex;
						break;
					}
				}

				// 위에서 어느 엣지를 기준으로 인접정보가 구성되었는지
				// 찾았다면 여기서 센터정보를 구해준다.
				//vecCenter.push_back(EdgeIndex);
				vecCenter.push_back(Cell->EdgeCenter[EdgeIndex]);
				// 삼각형의 중점을 넣어준다.
				//vecCenter.push_back(Cell->Center);

				m_PathList.push_back(Cell->EdgeCenter[EdgeIndex]);
			}

			// 얻어온 경로를 이용해서 가시성 판단을 한다.
			// 정해진 경로의 인덱스 수만큼 반복하며 해당 삼각형을
			// 관통하는지 판단한다.
			//Vector3	StartPos = Start;

			//for (size_t j = 0; j < vecCenter.size(); ++j)
			//{
			//	Vector3	Dir = vecCenter[j] - StartPos;
			//	float	fDist = Dir.Length();
			//	Dir.Normalize();

			//	Vector3	EndPos = StartPos + Dir * fDist;

			//	//if(!CheckPath(StartPos, EndPos))
			//	{
			//		if (j > 0)
			//			StartPos = vecCenter[j - 1];

			//		else
			//			StartPos = vecCenter[j];
			//		m_PathList.push_back(StartPos);
			//	}
			//}

			// 도착위치를 넣어준다.
			m_PathList.push_back(End);

			m_FindEnd = true;

			return;
		}

		//float	fG = Adj->Center.Distance(Cell->Center);
		//float	fH = Adj->Center.Distance(End);
		Vector3	Line = Adj->Center - Cell->Center;
		float	fG = Line.x * Line.x +
			Line.z * Line.z;

		Line = Adj->Center - End;
		float	fH = Line.x * Line.x +
			Line.z * Line.z;

		if (fG < 0.f || fH < 0.f)
		{
			MessageBox(nullptr, TEXT("aa"), TEXT("aa"), MB_OK);
		}

		fG = sqrtf(fG);
		fH = sqrtf(fH);

		if (Adj->Type == NCLT_NONE)
		{
			m_UseCellList[m_UseCellListSize] = Adj;
			++m_UseCellListSize;

			Adj->G = fG + Cell->G;
			Adj->H = fH;
			Adj->Total = Adj->G + Adj->H;
			Adj->ParentIdx = Cell->Index;
			Adj->Type = NCLT_OPEN;

			m_OpenList[m_OpenListSize] = Adj;
			++m_OpenListSize;
		}

		// 열린목록에 들어가있는 Cell일 경우
		else if (Adj->G > fG + Cell->G)
		{
			Adj->G = fG + Cell->G;
			Adj->H = fH;
			Adj->Total = Adj->G + Adj->H;
			Adj->ParentIdx = Cell->Index;
		}
	}
}

NavigationCell* CNavigationMesh::FindCell(const Vector3& Pos)
{
	int	idx = GetCellIndex(Pos);

	if (idx < 0 || idx >= m_vecCell.size())
		return nullptr;

	return m_vecCell[idx];
}

int CNavigationMesh::GetCellIndex(const Vector3& Pos)
{
	if (!m_Grid)
	{
		int	iSectionIndex = GetSectionIndex(Pos);

		if (0 <= iSectionIndex && iSectionIndex < m_SectionX * m_SectionZ)
		{
			Vector3	Origin = Pos;
			Origin.y = m_Max.y;
			Vector3	Dir = Vector3(0.f, -1.f, 0.f);

			for (int i = 0; i < m_Section[iSectionIndex].List.Size; ++i)
			{
				Vector3	Intersect;
				float	Dist = 0.f;
				NavigationCell*	Cell = m_Section[iSectionIndex].List.CellArray[i];

				if (RayIntersectTriangle(Origin, Dir, Cell->Pos[0],
					Cell->Pos[1],
					Cell->Pos[2],
					Dist, Intersect))
					return Cell->Index;
			}
		}

		return -1;
	}

	Vector3	CellSize = m_CellSize;
	Vector3	ConvertPos = Pos - m_Min;

	// 가로, 세로를 1로 만들어준다.
	ConvertPos.x /= CellSize.x;
	ConvertPos.z /= CellSize.z;

	// 사각형 인덱스를 구한다.
	int	idxX = (int)ConvertPos.x;
	int	idxZ = m_LineRectCount - (int)ConvertPos.z - 1;

	if (idxX < 0 || idxX >= m_LineRectCount)
		return -1;

	else if (idxZ < 0 || idxZ >= m_LineRectCount)
		return -1;

	float	X = ConvertPos.x - (int)ConvertPos.x;
	float	Z = 1.f - (ConvertPos.z - (int)ConvertPos.z);

	if (X >= Z)
		return (idxZ * m_LineRectCount * 2 + idxX * 2);

	return (idxZ * m_LineRectCount * 2 + idxX * 2) + 1;
}

bool CNavigationMesh::RayIntersectTriangle(Vector3 rayOrigin,
	Vector3 rayDir, Vector3 v0, Vector3 v1, Vector3 v2, float& t,
	Vector3& Intersect)
{
	Vector3 e1, e2, h, s, q;
	float a, f, u, v;

	e1 = v1 - v0;
	e2 = v2 - v0;
	h = rayDir.Cross(e2);
	a = e1.Dot(h);

	if (a > -0.00001 && a < 0.00001)
		return false;

	f = 1.f / a;
	s = rayOrigin - v0;

	u = f * s.Dot(h);

	if (u < 0.f || u > 1.f)
		return false;

	q = s.Cross(e1);

	v = f * rayDir.Dot(q);

	if (v < 0.f || u + v > 1.f)
		return false;

	t = f * e2.Dot(q);

	if (t > 0.00001)
	{
		Intersect = rayOrigin + rayDir * t;
		return true;
	}

	return false;
}

int CNavigationMesh::GetSectionIndex(Vector3 Pos)
{
	Pos -= m_Min;
	Pos.x /= m_CellSize.x;
	Pos.z /= m_CellSize.z;
	Pos /= m_SectionSize;

	int x, z;
	x = (int)Pos.x;
	z = (int)Pos.z;

	return z * m_SectionX + x;
}

void CNavigationMesh::Save(const char* FileName, const string& PathKey)
{
	const PathInfo* Path = CPathManager::GetInst()->FindPath(PathKey);

	string	strPath;

	if (Path)
		strPath = Path->PathMultibyte;

	strPath += FileName;

	SaveFromFullPath(strPath.c_str());
}

void CNavigationMesh::SaveFromFullPath(const char* FullPath)
{
	FILE* File = NULL;

	fopen_s(&File, FullPath, "wb");

	if (!File)
		return;

	fwrite(&m_Min, sizeof(Vector3), 1, File);
	fwrite(&m_Max, sizeof(Vector3), 1, File);
	fwrite(&m_Offset, sizeof(Vector3), 1, File);
	fwrite(&m_OffsetScale, sizeof(Vector3), 1, File);
	fwrite(&m_LineRectCount, sizeof(int), 1, File);
	fwrite(&m_Grid, sizeof(bool), 1, File);
	fwrite(&m_SectionX, sizeof(int), 1, File);
	fwrite(&m_SectionZ, sizeof(int), 1, File);
	fwrite(&m_SectionSize, sizeof(Vector3), 1, File);
	fwrite(&m_HeightGap, sizeof(float), 1, File);
	fwrite(&m_ArriveDist, sizeof(float), 1, File);
	fwrite(&m_CellSize, sizeof(Vector3), 1, File);

	for (int i = 0; i < m_SectionZ; ++i)
	{
		for (int j = 0; j < m_SectionX; ++j)
		{
			int	idx = i * m_SectionX + j;

			fwrite(&m_Section[idx].Size, sizeof(Vector3), 1, File);
			fwrite(&m_Section[idx].Min, sizeof(Vector3), 1, File);
			fwrite(&m_Section[idx].Max, sizeof(Vector3), 1, File);
		}
	}

	// 셀 수 저장
	size_t	Size = m_vecCell.size();

	fwrite(&Size, sizeof(size_t), 1, File);

	// 각각의 셀 정보 저장
	for (size_t i = 0; i < Size; ++i)
	{
		fwrite(m_vecCell[i]->Pos, sizeof(Vector3), 3, File);
		fwrite(m_vecCell[i]->Edge, sizeof(Vector3), 3, File);
		fwrite(m_vecCell[i]->EdgeCenter, sizeof(Vector3), 3, File);
		fwrite(&m_vecCell[i]->Center, sizeof(Vector3), 1, File);
		fwrite(&m_vecCell[i]->Index, sizeof(int), 1, File);
		fwrite(&m_vecCell[i]->Enable, sizeof(bool), 1, File);

		size_t	iAdjSize = m_vecCell[i]->vecAdj.size();

		fwrite(&iAdjSize, sizeof(size_t), 1, File);

		fwrite(&m_vecCell[i]->vecAdj[0], sizeof(AdjInfo), iAdjSize, File);
	}

	fclose(File);
}

void CNavigationMesh::Load(const char* FileName, const string& PathKey)
{
	const PathInfo* Path = CPathManager::GetInst()->FindPath(PathKey);

	string	strPath;

	if (Path)
		strPath = Path->PathMultibyte;

	strPath += FileName;

	LoadFromFullPath(strPath.c_str());
}

void CNavigationMesh::LoadFromFullPath(const char* FullPath)
{
	FILE* File = NULL;

	fopen_s(&File, FullPath, "rb");

	if (!File)
		return;

	fread(&m_Min, sizeof(Vector3), 1, File);
	fread(&m_Max, sizeof(Vector3), 1, File);
	fread(&m_Offset, sizeof(Vector3), 1, File);
	fread(&m_OffsetScale, sizeof(Vector3), 1, File);
	fread(&m_LineRectCount, sizeof(int), 1, File);
	fread(&m_Grid, sizeof(bool), 1, File);
	fread(&m_SectionX, sizeof(int), 1, File);
	fread(&m_SectionZ, sizeof(int), 1, File);
	fread(&m_SectionSize, sizeof(Vector3), 1, File);
	fread(&m_HeightGap, sizeof(float), 1, File);
	fread(&m_ArriveDist, sizeof(float), 1, File);
	fread(&m_CellSize, sizeof(Vector3), 1, File);

	SAFE_DELETE_ARRAY(m_Section);

	m_Section = new NavSection[m_SectionX * m_SectionZ];

	for (int i = 0; i < m_SectionZ; ++i)
	{
		for (int j = 0; j < m_SectionX; ++j)
		{
			int	idx = i * m_SectionX + j;

			fread(&m_Section[idx].Size, sizeof(Vector3), 1, File);
			fread(&m_Section[idx].Min, sizeof(Vector3), 1, File);
			fread(&m_Section[idx].Max, sizeof(Vector3), 1, File);
		}
	}

	// 셀 수 저장
	size_t	Size = m_vecCell.size();

	fread(&Size, sizeof(size_t), 1, File);

	auto	iter = m_vecCell.begin();
	auto	iterEnd = m_vecCell.end();

	for (; iter != iterEnd; ++iter)
	{
		SAFE_DELETE((*iter));
	}

	m_vecCell.clear();

	// 각각의 셀 정보 저장
	for (size_t i = 0; i < Size; ++i)
	{
		NavigationCell*	Cell = new NavigationCell;

		fread(Cell->Pos, sizeof(Vector3), 3, File);
		fread(Cell->Edge, sizeof(Vector3), 3, File);
		fread(Cell->EdgeCenter, sizeof(Vector3), 3, File);
		fread(&Cell->Center, sizeof(Vector3), 1, File);
		fread(&Cell->Index, sizeof(int), 1, File);
		fread(&Cell->Enable, sizeof(bool), 1, File);

		size_t	iAdjSize = 0;

		fread(&iAdjSize, sizeof(size_t), 1, File);

		Cell->vecAdj.clear();
		Cell->vecAdj.resize(iAdjSize);

		fread(&Cell->vecAdj[0], sizeof(AdjInfo), iAdjSize, File);

		m_vecCell.push_back(Cell);
	}

	fclose(File);

	AddSection();
}

bool CNavigationMesh::OpenListSort(const NavigationCell* pSrc,
	const NavigationCell* pDest)
{
	return pSrc->Total < pDest->Total;
}

bool CNavigationMesh::CheckPath(const Vector3& Start,
	const Vector3& End)
{
	int	StartX, StartY, EndX, EndY;
	StartX = Start.x;
	StartY = Start.z;
	EndX = End.x;
	EndY = End.z;

	int	dx = EndX - StartX;
	int	dy = EndY - StartY;

	int	iAddX = 1, iAddY = 1;

	if (dx < 0)
	{
		iAddX = -1;
		dx = -dx;
	}

	if (dy < 0)
	{
		iAddY = -1;
		dy = -dy;
	}

	int	x = StartX;
	int	y = StartY;
	int	iCount = 0;

	vector<int>	vecPathIdx;

	// 기울기가 1보다 작을 경우 x 증가량이 y증가량보다 크다는 것이다.
	if (dx >= dy)
	{
		for (int i = 0; i < dx; i += 1)
		{
			x += iAddX;

			iCount += dy;

			if (iCount >= dx)
			{
				y += iAddY;
				iCount -= dx;
			}

			int	iConvertY = m_LineRectCount - y - 1;

			if (x < 0 || x >= m_LineRectCount ||
				iConvertY < 0 || iConvertY >= m_LineRectCount)
				continue;

			int	iPathIdx = iConvertY * m_LineRectCount * 2 + x * 2;

			if (!m_vecCell[iPathIdx]->Enable)
			{
				int	a = 10;
			}

			if (!m_vecCell[iPathIdx + 1]->Enable)
			{
				int	a = 10;
			}

			// 사각형 인덱스를 구한다.
			vecPathIdx.push_back((iConvertY * m_LineRectCount * 2 + x * 2));
			vecPathIdx.push_back((iConvertY * m_LineRectCount * 2 + x * 2 + 1));
		}
	}

	else
	{
		for (int i = 0; i < dy; i += 1)
		{
			y += iAddY;

			iCount += dx;

			if (iCount >= dy)
			{
				x += iAddX;
				iCount -= dy;
			}

			int	iConvertY = m_LineRectCount - y - 1;

			if (x < 0 || x >= m_LineRectCount ||
				iConvertY < 0 || iConvertY >= m_LineRectCount)
				continue;

			int	iPathIdx = iConvertY * m_LineRectCount * 2 + x * 2;

			if (!m_vecCell[iPathIdx]->Enable)
			{
				int	a = 10;
			}

			if (!m_vecCell[iPathIdx + 1]->Enable)
			{
				int	a = 10;
			}

			// 사각형 인덱스를 구한다.
			vecPathIdx.push_back((iConvertY * m_LineRectCount * 2 + x * 2));
			vecPathIdx.push_back((iConvertY * m_LineRectCount * 2 + x * 2 + 1));
		}
	}

	for (size_t i = 0; i < vecPathIdx.size(); ++i)
	{
		// 직선이 해당 삼각형을 관통하는지
		// 판단하고 관통할 경우 해당 삼각형의 활성화 여부에 따라
		// 패스를 구해준다.
		int	idx = vecPathIdx[i];

		for (int j = 0; j < 3; ++j)
		{
			Vector3	EdgeStart, EdgeEnd;

			switch (j)
			{
			case 0:
				EdgeStart = m_vecCell[idx]->Pos[0];
				EdgeEnd = m_vecCell[idx]->Pos[1];
				break;
			case 1:
				EdgeStart = m_vecCell[idx]->Pos[1];
				EdgeEnd = m_vecCell[idx]->Pos[2];
				break;
			case 2:
				EdgeStart = m_vecCell[idx]->Pos[0];
				EdgeEnd = m_vecCell[idx]->Pos[2];
				break;
			}

			EdgeStart.y = 0.f;
			EdgeEnd.y = 0.f;

			Vector3	StartPos = Start;
			Vector3	EndPos = End;

			StartPos.y = 0.f;
			EndPos.y = 0.f;

			Vector3	vIntersect;
			if (CheckPathDir(StartPos, EndPos, EdgeStart,
				EdgeEnd, vIntersect))
			{
				if (!m_vecCell[idx]->Enable)
					return false;

				break;
			}
		}
	}

	return true;
}

bool CNavigationMesh::CheckPathAll(const Vector3& Start,
	const Vector3& End)
{
	int	StartX, StartY, EndX, EndY;
	StartX = Start.x;
	StartY = Start.z;
	EndX = End.x;
	EndY = End.z;

	int	dx = EndX - StartX;
	int	dy = EndY - StartY;

	int	iAddX = 1, iAddY = 1;

	if (dx < 0)
	{
		iAddX = -1;
		dx = -dx;
	}

	if (dy < 0)
	{
		iAddY = -1;
		dy = -dy;
	}

	int	x = StartX;
	int	y = StartY;
	int	iCount = 0;

	vector<int>	vecPathIdx;

	// 기울기가 1보다 작을 경우 x 증가량이 y증가량보다 크다는 것이다.
	if (dx >= dy)
	{
		for (int i = 0; i < dx; i += 1)
		{
			x += iAddX;

			iCount += dy;

			if (iCount >= dx)
			{
				y += iAddY;
				iCount -= dx;
			}

			int	iConvertY = m_LineRectCount - y - 1;

			if (x < 0 || x >= m_LineRectCount ||
				iConvertY < 0 || iConvertY >= m_LineRectCount)
				continue;

			// 사각형 인덱스를 구한다.
			vecPathIdx.push_back((iConvertY * m_LineRectCount * 2 + x * 2));
			vecPathIdx.push_back((iConvertY * m_LineRectCount * 2 + x * 2 + 1));
		}
	}

	else
	{
		for (int i = 0; i < dy; i += 1)
		{
			y += iAddY;

			iCount += dx;

			if (iCount >= dy)
			{
				x += iAddX;
				iCount -= dy;
			}

			int	iConvertY = m_LineRectCount - y - 1;

			if (x < 0 || x >= m_LineRectCount ||
				iConvertY < 0 || iConvertY >= m_LineRectCount)
				continue;

			// 사각형 인덱스를 구한다.
			vecPathIdx.push_back((iConvertY * m_LineRectCount * 2 + x * 2));
			vecPathIdx.push_back((iConvertY * m_LineRectCount * 2 + x * 2 + 1));
		}
	}

	for (size_t i = 0; i < vecPathIdx.size(); ++i)
	{
		// 직선이 해당 삼각형을 관통하는지
		// 판단하고 관통할 경우 해당 삼각형의 활성화 여부에 따라
		// 패스를 구해준다.
		int	idx = vecPathIdx[i];

		for (int j = 0; j < 3; ++j)
		{
			Vector3	EdgeStart, EdgeEnd;

			switch (j)
			{
			case 0:
				EdgeStart = m_vecCell[idx]->Pos[0];
				EdgeEnd = m_vecCell[idx]->Pos[1];
				break;
			case 1:
				EdgeStart = m_vecCell[idx]->Pos[2];
				EdgeEnd = m_vecCell[idx]->Pos[1];
				break;
			case 2:
				EdgeStart = m_vecCell[idx]->Pos[0];
				EdgeEnd = m_vecCell[idx]->Pos[2];
				break;
			}

			EdgeStart.y = 0.f;
			EdgeEnd.y = 0.f;

			Vector3	vIntersect;
			if (CheckPathDir(Start, End, EdgeStart,
				EdgeEnd, vIntersect))
			{
				if (!m_vecCell[idx]->Enable)
				{
					if (i == 0)
						m_LastPath = -1;

					else
						m_LastPath = vecPathIdx[i - 1];

					return false;
				}
			}
		}
	}

	return true;
}

bool CNavigationMesh::OpenListSort1(NavigationCell* pSrc, NavigationCell* pDest)
{
	return pSrc->Total < pDest->Total;
}

int CNavigationMesh::OpenListQSort(const void* _pSrc, const void* _pDest)
{
	NavigationCell*	pSrc = *(NavigationCell**)(_pSrc);
	NavigationCell*	pDest = *(NavigationCell**)(_pDest);

	if (pSrc->Total < pDest->Total)
		return 1;

	else if (pSrc->Total > pDest->Total)
		return -1;

	return 0;
}
