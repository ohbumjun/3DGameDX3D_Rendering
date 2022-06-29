#include "DefaultScene.h"
#include "Scene/Scene.h"
#include "Scene/SceneResource.h"
#include "Resource/Material/Material.h"

CDefaultScene::CDefaultScene()
{
    SetTypeID<CDefaultScene>();
}

CDefaultScene::~CDefaultScene()
{
}

bool CDefaultScene::Init()
{
    CreateMaterial();

    if (!LoadAnimationSequence2D())
        return false;

    return true;
}

bool CDefaultScene::LoadAnimationSequence2D()
{
    m_Scene->GetResource()->LoadSequence2D("PlayerIdle.sqc");
    m_Scene->GetResource()->LoadSequence2D("PlayerRun.sqc");

    return true;
}

bool CDefaultScene::CreateMaterial()
{
    m_Scene->GetResource()->CreateMaterial<CMaterial>("TileMap");
    CMaterial* Material = m_Scene->GetResource()->FindMaterial("TileMap");

    //Material->AddTexture(0, (int)Buffer_Shader_Type::Pixel, "DiabloTile", TEXT("Diablos_Lair_Floor_TRS/Diablos_Lair_Floor.png"));

    Material->SetShader("TileMapShader");
    Material->SetRenderState("AlphaBlend");

    return true;
}
