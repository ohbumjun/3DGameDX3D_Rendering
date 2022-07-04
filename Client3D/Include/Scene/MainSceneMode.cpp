
#include "MainSceneMode.h"
#include "Scene/Scene.h"
#include "Scene/SceneResource.h"
#include "Scene/Viewport.h"
#include "../Object/Player.h"
#include "../Object/LandScapeObj.h"
#include "../Object/DecalObj.h"
#include "../Object/Portal.h"
#include "../Object/BonObj.h"
#include "../Object/Monster.h"
#include "GameObject/LightObj.h"
#include "Component/LightComponent.h"
#include "Resource/Material/Material.h"

CMainSceneMode::CMainSceneMode()
{
	SetTypeID<CMainSceneMode>();
}

CMainSceneMode::~CMainSceneMode()
{
}

bool CMainSceneMode::Init()
{
	LoadMesh();

	CreateMaterial();

	CreateAnimationSequence();

	CreateParticle();

	if (m_LoadingFunction)
		m_LoadingFunction(false, 0.3f);

	CPlayer* Player = m_Scene->CreateGameObject<CPlayer>("Player");

	SetPlayerObject(Player);

	SphereInfo PlayerWorldSphereInfo = Player->GetRootComponent()->GetSphereInfo();

	CMonster* Monster = m_Scene->CreateGameObject<CMonster>("Monster");
	Monster->SetWorldPos(5.f, 0.f, 5.f);

	CLandScapeObj* LandScape = m_Scene->CreateGameObject<CLandScapeObj>("LandScape");

	CDecalObj* Decal = m_Scene->CreateGameObject<CDecalObj>("Decal");

	CPortal* Portal = m_Scene->CreateGameObject<CPortal>("Portal");

	CBonObj* BonObj = m_Scene->CreateGameObject<CBonObj>("BonObj");
	BonObj->SetWorldPos(3.f, 4.f, 10.f);

	// Static Mesh Instancing
	// for (int i = 0; i < 30; ++i)
	// {
	// 	CBonObj* BonObj = m_Scene->CreateGameObject<CBonObj>("BonObj");
	// 
	// 	BonObj->SetWorldPos(i * 3.f, 4.f, 10.f);
	// }


	// Animation Mesh Instancing
	// for (int i = 0; i < 5; ++i)
	// {
	// 	for (int j = 0; j < 5; ++j)
	// 	{
	// 		CMonster* Monster = m_Scene->CreateGameObject<CMonster>("Monster");
	// 
	// 		Monster->SetWorldPos(j * 3.f + 10.f, 5.f, i * 3.f + 10.f);
	// 	}
	// }



	CLightObj* Light = m_Scene->CreateGameObject<CLightObj>("Light1");

	((CLightComponent*)Light->GetRootComponent())->SetRelativePos(-3.f, 5.f, 0.f);
	((CLightComponent*)Light->GetRootComponent())->SetLightType(Light_Type::Point);
	((CLightComponent*)Light->GetRootComponent())->SetDistance(10.f);
	((CLightComponent*)Light->GetRootComponent())->SetAtt3(0.02f);
	((CLightComponent*)Light->GetRootComponent())->SetColor(Vector4(1.f, 0.f, 0.f, 1.f));

	CLightObj* Light2 = m_Scene->CreateGameObject<CLightObj>("Light2");

	((CLightComponent*)Light2->GetRootComponent())->SetRelativePos(3.f, 5.f, 0.f);
	((CLightComponent*)Light2->GetRootComponent())->SetLightType(Light_Type::Point);
	((CLightComponent*)Light2->GetRootComponent())->SetDistance(10.f);
	((CLightComponent*)Light2->GetRootComponent())->SetAtt3(0.02f);
	((CLightComponent*)Light2->GetRootComponent())->SetColor(Vector4(0.f, 1.f, 0.f, 1.f));
	
	return true;
}

void CMainSceneMode::LoadMesh()
{
	m_Scene->GetResource()->LoadMesh(Mesh_Type::Animation, "PlayerMesh",
		TEXT("Player_Default.msh"));


	m_Scene->GetResource()->LoadMesh(Mesh_Type::Static, "Blade",
		TEXT("Blade.msh"));


	m_Scene->GetResource()->LoadMesh(Mesh_Type::Static, "BonObj",
		TEXT("BonObj.msh"));

	CMesh* Mesh = m_Scene->GetResource()->FindMesh("PlayerMesh");

	/*CSharedPtr<CMaterial>	Mtrl = Mesh->GetMaterial(2);

	Mtrl->AddTexture(3, (int)Buffer_Shader_Type::Pixel,
		"CageEmv", TEXT("Player_Default.fbm/DXT1_cage003_EM.dds"),
		MESH_PATH);

	for (int i = 0; i < Mesh->GetMaterialSlots()->size(); ++i)
	{
		Mtrl = Mesh->GetMaterial(i);

		Mtrl->SetEmissiveColor(1.f, 1.f, 1.f, 1.f);
	}*/

	m_Scene->GetResource()->LoadSkeleton("PlayerSkeleton",
		TEXT("Player_Default.bne"), MESH_PATH);

	m_Scene->GetResource()->AddSocket("PlayerSkeleton", "bone11",
		"Weapon");

	m_Scene->GetResource()->SetMeshSkeleton("PlayerMesh", "PlayerSkeleton");

	m_Scene->GetResource()->LoadAnimationSequence(true, "PlayerIdle",
		TEXT("Player_Default.sqc"), MESH_PATH);

	m_Scene->GetResource()->LoadAnimationSequence(false, "PlayerAttack",
		TEXT("PlayerAttack.sqc"), MESH_PATH);

	m_Scene->GetResource()->LoadAnimationSequence(true, "PlayerWalk",
		TEXT("PlayerWalk.sqc"), MESH_PATH);
}

void CMainSceneMode::CreateMaterial()
{
	m_Scene->GetResource()->CreateMaterial<CMaterial>("LandScape");

	CMaterial* Material = m_Scene->GetResource()->FindMaterial("LandScape");

	Material->AddTexture(0, (int)Buffer_Shader_Type::Pixel,
		"LandScapeSplat1Dif", TEXT("LandScape/ROCK_01+MOSS_COLOR_1.png"));
	Material->AddTexture(1, (int)Buffer_Shader_Type::Pixel,
		"LandScapeSplat1Nrm", TEXT("LandScape/ROCK_01+MOSS_NRM.png"));
	Material->AddTexture(2, (int)Buffer_Shader_Type::Pixel,
		"LandScapeSplat1Spc", TEXT("LandScape/ROCK_01+MOSS_SPEC.png"));

	std::vector<TCHAR*>	vecDiffuseFileName;
	std::vector<TCHAR*>	vecNormalFileName;
	std::vector<TCHAR*>	vecSpecularFileName;
	std::vector<TCHAR*>	vecAlphaFileName;

	TCHAR* FileName = new TCHAR[MAX_PATH];
	memset(FileName, 0, sizeof(TCHAR) * MAX_PATH);
	lstrcpy(FileName, TEXT("LandScape/RoadAlpha1.bmp"));
	vecAlphaFileName.push_back(FileName);

	FileName = new TCHAR[MAX_PATH];
	memset(FileName, 0, sizeof(TCHAR) * MAX_PATH);
	lstrcpy(FileName, TEXT("LandScape/GrassFirstAlpha.bmp"));
	vecAlphaFileName.push_back(FileName);

	FileName = new TCHAR[MAX_PATH];
	memset(FileName, 0, sizeof(TCHAR) * MAX_PATH);
	lstrcpy(FileName, TEXT("LandScape/SandBaseAlpha.bmp"));
	vecAlphaFileName.push_back(FileName);

	FileName = new TCHAR[MAX_PATH];
	memset(FileName, 0, sizeof(TCHAR) * MAX_PATH);
	lstrcpy(FileName, TEXT("LandScape/WaterBaseAlpha.bmp"));
	vecAlphaFileName.push_back(FileName);


	FileName = new TCHAR[MAX_PATH];
	memset(FileName, 0, sizeof(TCHAR) * MAX_PATH);
	lstrcpy(FileName, TEXT("LandScape/BD_Terrain_Cave_01.dds"));
	vecDiffuseFileName.push_back(FileName);

	FileName = new TCHAR[MAX_PATH];
	memset(FileName, 0, sizeof(TCHAR) * MAX_PATH);
	lstrcpy(FileName, TEXT("LandScape/BD_Terrain_Cave_01_NRM.bmp"));
	vecNormalFileName.push_back(FileName);

	FileName = new TCHAR[MAX_PATH];
	memset(FileName, 0, sizeof(TCHAR) * MAX_PATH);
	lstrcpy(FileName, TEXT("LandScape/BD_Terrain_Cave_01_SPEC.bmp"));
	vecSpecularFileName.push_back(FileName);


	FileName = new TCHAR[MAX_PATH];
	memset(FileName, 0, sizeof(TCHAR) * MAX_PATH);
	lstrcpy(FileName, TEXT("LandScape/BD_Terrain_Cliff05.dds"));
	vecDiffuseFileName.push_back(FileName);

	FileName = new TCHAR[MAX_PATH];
	memset(FileName, 0, sizeof(TCHAR) * MAX_PATH);
	lstrcpy(FileName, TEXT("LandScape/BD_Terrain_Cliff05_NRM.bmp"));
	vecNormalFileName.push_back(FileName);

	FileName = new TCHAR[MAX_PATH];
	memset(FileName, 0, sizeof(TCHAR) * MAX_PATH);
	lstrcpy(FileName, TEXT("LandScape/BD_Terrain_Cliff05_SPEC.bmp"));
	vecSpecularFileName.push_back(FileName);


	FileName = new TCHAR[MAX_PATH];
	memset(FileName, 0, sizeof(TCHAR) * MAX_PATH);
	lstrcpy(FileName, TEXT("LandScape/Terrain_Cliff_15_Large.dds"));
	vecDiffuseFileName.push_back(FileName);

	FileName = new TCHAR[MAX_PATH];
	memset(FileName, 0, sizeof(TCHAR) * MAX_PATH);
	lstrcpy(FileName, TEXT("LandScape/Terrain_Cliff_15_Large_NRM.bmp"));
	vecNormalFileName.push_back(FileName);

	FileName = new TCHAR[MAX_PATH];
	memset(FileName, 0, sizeof(TCHAR) * MAX_PATH);
	lstrcpy(FileName, TEXT("LandScape/Terrain_Cliff_15_Large_SPEC.bmp"));
	vecSpecularFileName.push_back(FileName);


	FileName = new TCHAR[MAX_PATH];
	memset(FileName, 0, sizeof(TCHAR) * MAX_PATH);
	lstrcpy(FileName, TEXT("LandScape/Terrain_Pebbles_01.dds"));
	vecDiffuseFileName.push_back(FileName);

	FileName = new TCHAR[MAX_PATH];
	memset(FileName, 0, sizeof(TCHAR) * MAX_PATH);
	lstrcpy(FileName, TEXT("LandScape/Terrain_Pebbles_01_NRM.bmp"));
	vecNormalFileName.push_back(FileName);

	FileName = new TCHAR[MAX_PATH];
	memset(FileName, 0, sizeof(TCHAR) * MAX_PATH);
	lstrcpy(FileName, TEXT("LandScape/Terrain_Pebbles_01_SPEC.bmp"));
	vecSpecularFileName.push_back(FileName);

	Material->AddTextureArray(30, (int)Buffer_Shader_Type::Pixel,
		"SplatDiffuse", vecDiffuseFileName);
	Material->AddTextureArray(31, (int)Buffer_Shader_Type::Pixel,
		"SplatNormal", vecNormalFileName);
	Material->AddTextureArray(32, (int)Buffer_Shader_Type::Pixel,
		"SplatSpecular", vecSpecularFileName);
	Material->AddTextureArray(33, (int)Buffer_Shader_Type::Pixel,
		"SplatAlpha", vecAlphaFileName);

	SAFE_DELETE_ARRAY_VECLIST(vecDiffuseFileName);
	SAFE_DELETE_ARRAY_VECLIST(vecNormalFileName);
	SAFE_DELETE_ARRAY_VECLIST(vecSpecularFileName);
	SAFE_DELETE_ARRAY_VECLIST(vecAlphaFileName);

	Material->SetShader("LandScapeShader");

	Material->EnableBump();
	Material->EnableSpecularTex();
	Material->SetSpecularPower(2.5f);
}

void CMainSceneMode::CreateAnimationSequence()
{
}

void CMainSceneMode::CreateParticle()
{
}
