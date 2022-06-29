#pragma once

#include "../Engine.h"
#include <stack>

using namespace std;

enum NAVIMESH_CELL_LIST_TYPE
{
	NCLT_NONE,
	NCLT_OPEN,
	NCLT_CLOSE
};

struct AdjInfo
{
	int	Index;
	int	EdgeIndex;
};

struct NavigationCell
{
	NAVIMESH_CELL_LIST_TYPE	Type;
	Vector3		Pos[3];
	Vector3		Edge[3];
	Vector3		EdgeCenter[3];
	Vector3		Center;
	vector<AdjInfo>	vecAdj;
	int			Index;
	int			ParentIdx;
	float		G;
	float		H;
	float		Total;
	bool		Enable;

	void Clear()
	{
		Type = NCLT_NONE;
		ParentIdx = -1;
		G = -1.f;
		H = -1.f;
		Total = -1.f;
	}

	NavigationCell()
	{
		Type = NCLT_NONE;
		ParentIdx = -1;
		Index = -1;
		G = -1.f;
		H = -1.f;
		Total = -1.f;
		Enable = true;
	}

	NavigationCell operator + (const NavigationCell& cell)
	{
		NavigationCell	_cell;

		return _cell;
	}
};

struct CellList
{
	NavigationCell** CellArray;
	int			Size;
	int			Capacity;

	void Resize()
	{
		if (Size == Capacity)
		{
			Capacity *= 2;
			NavigationCell** pList = new NavigationCell * [Capacity];

			memset(pList, 0, sizeof(NavigationCell*) * Capacity);

			memcpy(pList, CellArray, sizeof(NavigationCell*) * Size);

			SAFE_DELETE_ARRAY(CellArray);
			CellArray = pList;
		}
	}

	void Add(NavigationCell* pCell)
	{
		Resize();
		CellArray[Size] = pCell;
		++Size;
	}

	CellList()
	{
		Size = 0;
		Capacity = 2048;
		CellArray = new NavigationCell * [Capacity];

		memset(CellArray, 0, sizeof(NavigationCell*) * Capacity);
	}

	~CellList()
	{
		SAFE_DELETE_ARRAY(CellArray);
	}
};

struct NavSection
{
	Vector3		Size;
	CellList	List;
	Vector3		Min;
	Vector3		Max;

	void Add(NavigationCell* pCell)
	{
		List.Add(pCell);
	}

	NavSection()
	{
	}

	~NavSection()
	{
	}
};

class CNavigationMesh
{
	friend class CNavigation3D;
	friend class CNavigationThread3D;
	friend class CNavigation3DManager;

private:
	CNavigationMesh();
	~CNavigationMesh();

private:
	string		m_Name;
	vector<NavigationCell*>	m_vecCell;
	NavigationCell** m_UseCellList;
	int						m_UseCellListSize;
	Vector3		m_Offset;
	Vector3		m_OffsetScale;
	Vector3		m_Min;
	Vector3		m_Max;
	int			m_LineRectCount;
	bool		m_Grid;
	int			m_SectionX;
	int			m_SectionZ;
	Vector3		m_SectionSize;
	NavSection*	m_Section;
	float			m_HeightGap;
	float			m_ArriveDist;
	Vector3	m_CellSize;

public:
	void SetCellSize(float fX, float fZ);

public:
	void ComputeArriveDist(float fX, float fY);
	float GetArriveDist()	const;

private:
	NavigationCell** m_OpenList;
	int								m_OpenListSize;
	stack<int>					m_FindStack;
	list<Vector3>				m_PathList;
	bool								m_FindEnd;
	int								m_LastPath;
	NavigationCell* m_StartCell;
	Vector3						m_Start;

public:
	Vector3	GetMin()	const;
	Vector3 GetMax()	const;
	Vector3 GetOffset()	const;
	list<Vector3> GetPathList()	const;

public:
	string GetName()	const;

public:
	void SetName(const string& Name);
	void SetOffset(const Vector3& Offset);
	void SetOffsetScale(const Vector3& OffsetScale);

public:
	bool Init();
	void ClearCell();
	void AddCell(const Vector3 Pos[3]);
	void AddAdj(int CellIdx, int AdjIdx);
	void CreateGridMapAdj(int LineRectCount);
	void CreateAdj();
	bool CheckOnEdge(int Src, int Dest, const Vector3& Origin1, const Vector3& Origin2,
		const Vector3& Edge, float EdgeLength, int Edge1, int Edge2);
	float ccw(const Vector2& v1, const Vector2& v2);
	float ccw(const Vector2& v1, const Vector2& v2, const Vector2& v3);
	bool CheckPathDir(const Vector3& Src1, const Vector3& Src2,
		const Vector3& Dest1, const Vector3& Dest2, Vector3& Intersect);
	void FindPath(Vector3 Start, const Vector3& End);

public:
	float GetY(const Vector3& Pos);
	bool CheckCell(const Vector3& Pos);
	bool GetCellEnable(const Vector3& Pos);
	float GetY(int CellIndex, const Vector3& Pos);
	void CreateSection();
	void AddSection();
	bool RayIntersectTriangle(Ray& tRay);

private:
	void AddOpenList(NavigationCell* Cell, NavigationCell* EndCell,
		Vector3 Start, const Vector3& End);
	NavigationCell* FindCell(const Vector3& Pos);
	int GetCellIndex(const Vector3& Pos);
	bool RayIntersectTriangle(Vector3 rayOrigin, Vector3 rayDir,
		Vector3 v0, Vector3 v1, Vector3 v2,
		float& t, Vector3& Intersect);
	int GetSectionIndex(Vector3 Pos);

public:
	void Save(const char* pFileName, const string& PathKey = NAVIGATION_PATH);
	void SaveFromFullPath(const char* pFullPath);
	void Load(const char* pFileName, const string& PathKey = NAVIGATION_PATH);
	void LoadFromFullPath(const char* pFullPath);

private:
	bool OpenListSort(const NavigationCell* Src,
		const NavigationCell* Dest);
	bool CheckPath(const Vector3& Start, const Vector3& End);
	bool CheckPathAll(const Vector3& Start, const Vector3& End);
	static bool OpenListSort1(NavigationCell* Src,
		NavigationCell* Dest);
	static int OpenListQSort(const void* _Src, const void* _Dest);
};

