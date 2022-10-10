#pragma once

#include "Mesh.h"

class CSprite3DMesh :
    public CMesh
{
	friend class CMeshManager;

protected:
	CSprite3DMesh();
	virtual ~CSprite3DMesh();

public:
	virtual bool Init();
};

