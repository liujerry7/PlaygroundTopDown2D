#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "Engine/Core/EngineCommon.hpp"
#include "Game/App.hpp"
#include "Game/GameCommon.hpp"

int WINAPI WinMain(_In_ HINSTANCE applicationInstanceHandle, _In_opt_ HINSTANCE, _In_ LPSTR commandLineString, _In_ int)
{
	UNUSED(applicationInstanceHandle);
	UNUSED(commandLineString);

	g_theApp = new App();
	g_theApp->Startup();
	g_theApp->RunMainLoop();
	g_theApp->Shutdown();

	delete g_theApp;
	g_theApp = nullptr;

	return 0;
}
