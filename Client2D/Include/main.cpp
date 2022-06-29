
#include "ClientManager.h"

#ifdef _DEBUG

#pragma comment(lib, "GameEngine_Debug.lib")

#else

#pragma comment(lib, "GameEngine.lib")

#endif // _DEBUG


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	if (!CClientManager::GetInst()->Init(hInstance))
	{
		CClientManager::DestroyInst();
		return 0;
	}

	CClientManager::GetInst()->CreateDefaultSceneMode();

	int Ret = CClientManager::GetInst()->Run();

	CClientManager::DestroyInst();

	return Ret;
}
