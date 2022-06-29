#include "ClientManager3D.h"
#include "Engine.h"
#include "resource.h"
#include "Scene/SceneManager.h"
#include "Scene/MainSceneMode.h"
//#include "Scene/StartScene.h"
#include "Input.h"
#include "Resource/ResourceManager.h"
//#include "Widget/MouseNormal.h"
//#include "Widget/MouseAttack.h"
//#include "Object/Player2D.h"
//#include "Object/TileMap.h"
//#include "Component/TileMapComponent.h"
//#include "Component/StaticMeshComponent.h"
//#include "Component/SpriteComponent.h"

DEFINITION_SINGLE(CClientManager3D)

CClientManager3D::CClientManager3D()
{
}

CClientManager3D::~CClientManager3D()
{
	CEngine::DestroyInst();
}

bool CClientManager3D::Init(HINSTANCE hInst)
{
	if (!CEngine::GetInst()->Init(hInst, TEXT("GameEngine"),
		1280, 720, IDI_ICON1))
	{
		CEngine::DestroyInst();
		return false;
	}

	CSceneManager::GetInst()->SetCreateSceneModeFunction<CClientManager3D>(this, &CClientManager3D::CreateSceneMode);
	CSceneManager::GetInst()->SetCreateObjectFunction<CClientManager3D>(this, &CClientManager3D::CreateObject);
	CSceneManager::GetInst()->SetCreateComponentFunction<CClientManager3D>(this, &CClientManager3D::CreateComponent);

	CInput::GetInst()->CreateKey("MoveFront", 'W');
	CInput::GetInst()->CreateKey("MoveBack", 'S');
	CInput::GetInst()->CreateKey("RotationYInv", 'A');
	CInput::GetInst()->CreateKey("RotationY", 'D');
	CInput::GetInst()->CreateKey("Attack", VK_SPACE);
	CInput::GetInst()->CreateKey("Attack1", VK_LBUTTON);
	CInput::GetInst()->CreateKey("Skill1", '1');
	CInput::GetInst()->CreateKey("MovePoint", VK_RBUTTON);

	CResourceManager::GetInst()->CreateSoundChannelGroup("UI");
	CResourceManager::GetInst()->SetVolume(10);

	// 마우스 위젯 설정
	//CMouseNormal* MouseNormal = CEngine::GetInst()->CreateMouse<CMouseNormal>(Mouse_State::Normal, "MouseNormal");
	//CMouseAttack* MouseAttack = CEngine::GetInst()->CreateMouse<CMouseAttack>(Mouse_State::State1, "MouseAttack");

	return true;
}

void CClientManager3D::CreateDefaultSceneMode()
{
	CSceneManager::GetInst()->CreateSceneMode<CMainSceneMode>();
}

int CClientManager3D::Run()
{
	return CEngine::GetInst()->Run();
}

void CClientManager3D::CreateSceneMode(CScene* Scene, size_t Type)
{
}

CGameObject* CClientManager3D::CreateObject(CScene* Scene, size_t Type)
{
	/*if (Type == typeid(CGameObject).hash_code())
	{
		CGameObject* Obj = Scene->LoadGameObject<CGameObject>();

		return Obj;
	}

	else if (Type == typeid(CPlayer2D).hash_code())
	{
		CPlayer2D* Obj = Scene->LoadGameObject<CPlayer2D>();

		return Obj;
	}

	else if (Type == typeid(CTileMap).hash_code())
	{
		CTileMap* Obj = Scene->LoadGameObject<CTileMap>();

		return Obj;
	}*/

	return nullptr;
}

CComponent* CClientManager3D::CreateComponent(CGameObject* Obj, size_t Type)
{
	/*if (Type == typeid(CSceneComponent).hash_code())
	{
		CComponent* Component = Obj->LoadComponent<CSceneComponent>();

		return Component;
	}

	else if (Type == typeid(CSpriteComponent).hash_code())
	{
		CComponent* Component = Obj->LoadComponent<CSpriteComponent>();

		return Component;
	}

	else if (Type == typeid(CStaticMeshComponent).hash_code())
	{
		CComponent* Component = Obj->LoadComponent<CStaticMeshComponent>();

		return Component;
	}

	else if (Type == typeid(CTileMapComponent).hash_code())
	{
		CTileMapComponent* Component = Obj->LoadComponent<CTileMapComponent>();

		Component->EnableEditMode(true);

		return Component;
	}*/

	return nullptr;
}
