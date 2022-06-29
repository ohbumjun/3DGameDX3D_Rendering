
#include "MainScene.h"
#include "Scene/Scene.h"
#include "Scene/Viewport.h"
#include "../Object/Player2D.h"
#include "../Object/Monster.h"
#include "../Object/PixelTest.h"
#include "Scene/SceneResource.h"
#include "Resource/Particle/Particle.h"
#include "../Object/BubbleParticle.h"
#include "../Object/TileMap.h"

CMainScene::CMainScene()
{
	SetTypeID<CMainScene>();
}

CMainScene::~CMainScene()
{
}

bool CMainScene::Init()
{
	CreateMaterial();

	CreateAnimationSequence();

	CreateParticle();

	if (m_LoadingFunction)
		m_LoadingFunction(false, 0.3f);

	CPlayer2D* Player = m_Scene->CreateGameObject<CPlayer2D>("Player");

	SetPlayerObject(Player);

	if (m_LoadingFunction)
		m_LoadingFunction(false, 0.6f);

	CMonster* Monster = m_Scene->CreateGameObject<CMonster>("Monster");

	if (m_LoadingFunction)
		m_LoadingFunction(false, 0.8f);

	//CTileMap* TileMap = m_Scene->CreateGameObject<CTileMap>("TileMap");
	CTileMap* TileMap = m_Scene->LoadGameObject<CTileMap>();

	//TileMap->Load("TileMapObj.gobj", SCENE_PATH);
	//TileMap->Load("MyTile.gobj", SCENE_PATH);
	TileMap->Load("RhombusTile.gobj", SCENE_PATH);

	CPixelTest* PixelTest = m_Scene->CreateGameObject<CPixelTest>("PixelTest");

	CBubbleParticle* BubbleParticle = m_Scene->CreateGameObject<CBubbleParticle>("BubbleParticle");

	BubbleParticle = m_Scene->CreateGameObject<CBubbleParticle>("BubbleParticle");

	BubbleParticle->SetRelativePos(-100.f, 0.f, 0.f);

	/*for (int i = 0; i < 100; ++i)
	{
		BubbleParticle = m_Scene->CreateGameObject<CBubbleParticle>("BubbleParticle");
		BubbleParticle->SetRelativePos(-100.f + i * 10.f, 0.f, 0.f);
	}

	BubbleParticle = m_Scene->CreateGameObject<CBubbleParticle>("BubbleParticle");
	BubbleParticle->SetRelativePos(-200.f, 0.f, 0.f);

	BubbleParticle = m_Scene->CreateGameObject<CBubbleParticle>("BubbleParticle");
	BubbleParticle->SetRelativePos(-300.f, 0.f, 0.f);*/

	m_MainWidget = m_Scene->GetViewport()->CreateWidgetWindow<CMainWidget>("MainWidget");

	return true;
}

void CMainScene::CreateMaterial()
{
	m_Scene->GetResource()->CreateMaterial<CMaterial>("Bubble");
	CMaterial* Material = m_Scene->GetResource()->FindMaterial("Bubble");
	
	Material->AddTexture(0, (int)Buffer_Shader_Type::Pixel, "Bubble", TEXT("Particle/Bubbles99px.png"));

	Material->SetShader("ParticleRenderShader");
	Material->SetRenderState("AlphaBlend");
	//Material->SetTransparency(true);

	m_Scene->GetResource()->CreateMaterial<CMaterial>("TileMap");
	Material = m_Scene->GetResource()->FindMaterial("TileMap");

	Material->AddTexture(0, (int)Buffer_Shader_Type::Pixel, "Tile", TEXT("Floors.png"));

	Material->SetShader("TileMapShader");
	Material->SetRenderState("AlphaBlend");

	m_Scene->GetResource()->CreateMaterial<CMaterial>("DiabloTileMap");
	Material = m_Scene->GetResource()->FindMaterial("DiabloTileMap");

	Material->AddTexture(0, (int)Buffer_Shader_Type::Pixel, "DiabloTile", TEXT("Diablos_Lair_Floor_TRS/Diablos_Lair_Floor.png"));

	Material->SetShader("TileMapShader");
	Material->SetRenderState("AlphaBlend");
}

void CMainScene::CreateAnimationSequence()
{
	m_Scene->GetResource()->CreateAnimationSequence2D("PlayerIdle", "Player", TEXT("Player.png"));

	for (int i = 0; i < 7; ++i)
	{
		m_Scene->GetResource()->AddAnimationSequence2DFrame("PlayerIdle", Vector2(i * 50.f, 148.f), Vector2(50.f, 37.f));
	}
}

void CMainScene::CreateParticle()
{
	m_Scene->GetResource()->CreateParticle("Bubble");
	CParticle* Particle = m_Scene->GetResource()->FindParticle("Bubble");

	CMaterial* Material = m_Scene->GetResource()->FindMaterial("Bubble");

	Particle->SetMaterial(Material);
	Particle->SetSpawnCountMax(1000);
	Particle->SetLifeTimeMin(3.f);
	Particle->SetLifeTimeMax(5.f);
	Particle->SetScaleMin(Vector3(20.f, 20.f, 1.f));
	Particle->SetScaleMax(Vector3(50.f, 50.f, 1.f));
	Particle->SetSpeedMin(100.f);
	Particle->SetSpeedMax(300.f);
	Particle->SetMoveDir(Vector3(0.f, 1.f, 0.f));
	Particle->SetStartMin(Vector3(-30.f, -30.f, 0.f));
	Particle->SetStartMax(Vector3(30.f, 30.f, 0.f));
	Particle->SetColorMin(Vector4(0.2f, 0.1f, 0.8f, 1.f));
	Particle->SetColorMax(Vector4(0.2f, 0.1f, 0.8f, 1.f));
	Particle->SetMoveAngle(Vector3(0.f, 0.f, 30.f));
	Particle->SetGravity(true);
	Particle->SetMove(true);
}
