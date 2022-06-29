#pragma once

#include "SceneComponent.h"
#include "../Resource/Mesh/SpriteMesh.h"
#include "../Resource/Material/Material.h"

class CBillboardComponent :
    public CSceneComponent
{
	friend class CGameObject;
	friend class CCameraManager;

protected:
	CBillboardComponent();
	CBillboardComponent(const CBillboardComponent& com);
	virtual ~CBillboardComponent();

protected:
	CSharedPtr<CSpriteMesh> m_Mesh;
	CSharedPtr<CMaterial> m_Material;

public:
	void SetMaterial(const std::string& Name);
	void SetMaterial(CMaterial* Material);

public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);
	virtual void PrevRender();
	virtual void Render();
	virtual void RenderDebug();
	virtual void PostRender();
	virtual CBillboardComponent* Clone();
	virtual void Save(FILE* File);
	virtual void Load(FILE* File);
};

