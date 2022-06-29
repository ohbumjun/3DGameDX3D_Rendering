
#include "ClientManager3D.h"

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
	if (!CClientManager3D::GetInst()->Init(hInstance))
	{
		CClientManager3D::DestroyInst();
		return 0;
	}

	CClientManager3D::GetInst()->CreateDefaultSceneMode();

	int Ret = CClientManager3D::GetInst()->Run();

	CClientManager3D::DestroyInst();

	return Ret;
}
