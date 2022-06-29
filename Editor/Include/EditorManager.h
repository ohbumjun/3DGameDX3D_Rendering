#pragma once

#include "GameInfo.h"

enum class EditMode
{
	Scene,
	Sprite,
	TileMap
};

class CEditorManager
{
private:
	EditMode	m_EditMode;
	class CDragObject* m_DragObj;
	class CSpriteWindow* m_SpriteWindow;
	class CDetailWindow* m_DetailWindow;
	class CEditorMenu* m_EditorMenu;
	class CObjectHierarchy* m_ObjectHierarchy;
	class CTileMapWindow* m_TileMapWindow;

	bool				m_MousePush;
	float				m_CameraMoveSpeed;

public:
	bool GetLButtonPush()	const
	{
		return m_MousePush;
	}

	class CDragObject* GetDragObj()	const
	{
		return m_DragObj;
	}

	EditMode GetEditMode()
	{
		return m_EditMode;
	}

	void SetEditMode(EditMode Mode);

public:
	bool Init(HINSTANCE hInst);
	void CreateDefaultSceneMode();
	int Run();

public:
	void MouseLButtonDown(float DeltaTime);
	void MouseLButtonPush(float DeltaTime);
	void MouseLButtonUp(float DeltaTime);

	void KeyboardUp(float DeltaTime);
	void KeyboardDown(float DeltaTime);
	void KeyboardLeft(float DeltaTime);
	void KeyboardRight(float DeltaTime);

public:
	void CreateSceneMode(class CScene* Scene, size_t Type);
	class CGameObject* CreateObject(class CScene* Scene, size_t Type);
	class CComponent* CreateComponent(class CGameObject* Obj, size_t Type);
	void CreateAnimInstance(class CSpriteComponent* Sprite, size_t Type);

	DECLARE_SINGLE(CEditorManager)
};

