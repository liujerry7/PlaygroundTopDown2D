#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "Engine/Core/EngineCommon.hpp"
#include "Game/App.hpp"

int WINAPI WinMain(HINSTANCE applicationInstanceHandle, HINSTANCE, LPSTR commandLineString, int)
{
	UNUSED(applicationInstanceHandle);
	UNUSED(commandLineString);

	g_app = new App();

	while (!g_app->IsQuitting())
	{
		g_app->RunFrame();
	}

	delete g_app;
	g_app = nullptr;

	return 0;
}
