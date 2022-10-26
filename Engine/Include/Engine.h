#pragma once

#include "GameInfo.h"
#include "Timer.h"

class CEngine
{
private:
	HINSTANCE	m_hInst;
	HWND		m_hWnd;
	Resolution	m_RS;
	static bool	m_Loop;
	float		m_ClearColor[4];
	class CTimer* m_Timer;
	bool		m_Start;
	bool		m_Play;
	Engine_Space	m_Space;
	int				m_ShowCursorCount;
	Mouse_State		m_MouseState;
	CSharedPtr<class CWidgetWindow>	m_MouseWidget[(int)Mouse_State::Max];
	class CGlobalConstantBuffer* m_GlobalCBuffer;
	float			m_GlobalAccTime;
	CSharedPtr<class CTexture>	m_GlobalNoiseTexture;
	class CStructuredBuffer* m_RandomBuffer;
	bool			m_EditMode;
	bool          m_ShowPickingLayer;
	bool          m_ShowColliderLayer;
public:
	void SetMouseState(Mouse_State State);

	Resolution GetResolution() const
	{
		return m_RS;
	}

	bool IsPickingLayerShowEnable() const
	{
		return m_ShowPickingLayer;
	}

	bool IsColliderLayerShowEnable() const
	{
		return m_ShowColliderLayer;
	}

	void EnableEditMode()
	{
		m_EditMode = true;
	}

	bool GetEditMode()	const
	{
		return m_EditMode;
	}

	class CWidgetWindow* GetMouseWidget()	const
	{
		return m_MouseWidget[(int)m_MouseState];
	}

	Engine_Space GetEngineSpace()	const
	{
		return m_Space;
	}

	void SetShowColliderLayerEnable(bool Enable)
	{
		m_ShowColliderLayer = Enable;
	}

	void SetShowPickingLayerEnable(bool Enable)
	{
		m_ShowPickingLayer = Enable;
	}

	void SetEngineSpace(Engine_Space Space)
	{
		m_Space = Space;
	}

	bool IsPlay()	const
	{
		return m_Play;
	}

	void SetPlay(bool Play)
	{
		m_Play = Play;
	}

	HWND GetWindowHandle()	const
	{
		return m_hWnd;
	}

	HINSTANCE GetWindowInstance()	const
	{
		return m_hInst;
	}

	void Exit()
	{
		DestroyWindow(m_hWnd);
	}

	float GetFPS()	const
	{
		return m_Timer->GetFPS();
	}

	float GetDeltaTime()	const
	{
		return m_Timer->GetDeltaTime();
	}

public:
	bool Init(HINSTANCE hInst, const TCHAR* Name, unsigned int Width,
		unsigned int Height, int IconID,
		bool WindowMode = true);
	bool Init(HINSTANCE hInst, HWND hWnd, unsigned int Width,
		unsigned int Height, bool WindowMode = true);
	int Run();
	void Logic();

private:
	bool Update(float DeltaTime);
	bool PostUpdate(float DeltaTime);
	bool Render(float DeltaTime);

private:
	ATOM Register(const TCHAR* Name, int IconID);
	BOOL Create(const TCHAR* Name);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
	template <typename T>
	T* CreateMouse(Mouse_State State, const std::string& Name)
	{
		T* Window = new T;

		Window->SetName(Name);

		if (!Window->Init())
		{
			SAFE_RELEASE(Window);
			return nullptr;
		}

		m_MouseWidget[(int)State] = Window;

		return Window;
	}

	DECLARE_SINGLE(CEngine)
};

