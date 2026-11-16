#include "Game/App.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"

App::App()
{
}

App::~App()
{
}

void App::Startup()
{
	EngineConfig config;
	config.m_windowConfig.m_windowTitle  = "PlaygroundTopDown2D";
	config.m_windowConfig.m_clientAspect = SCREEN_SIZE_X / SCREEN_SIZE_Y;

	new Engine(config);

	g_theRenderer = g_engine->m_renderer;
	g_theInput    = g_engine->m_inputSystem;
	g_theAudio    = g_engine->m_audioSystem;
	g_theWindow   = g_engine->m_window;

	m_theGame = new Game();
}

void App::Shutdown()
{
	delete m_theGame;
	m_theGame = nullptr;

	g_theRenderer = nullptr;
	g_theInput    = nullptr;
	g_theAudio    = nullptr;
	g_theWindow   = nullptr;

	delete g_engine;
}

void App::RunMainLoop()
{
	while (!IsQuitting())
	{
		RunFrame();
	}
}

void App::RunFrame()
{
	BeginFrame();
	Update();
	Render();
	EndFrame();
}

bool App::IsQuitting() const
{
	return m_isQuitting;
}

void App::HandleQuitRequested()
{
	m_isQuitting = true;
}

void App::BeginFrame()
{
	g_engine->BeginFrame();
	m_gameClock.BeginFrame();

	if (g_theWindow->m_isQuitRequested)
	{
		HandleQuitRequested();
	}
}

void App::Update()
{
	HandleDeveloperKeys();
	m_theGame->Update(m_gameClock.GetDeltaSeconds());
}

void App::Render() const
{
	m_theGame->Render();
}

void App::EndFrame()
{
	m_gameClock.EndFrame();
	g_engine->EndFrame();
}

void App::HandleDeveloperKeys()
{
	if (g_theInput->WasKeyJustPressed(KEYCODE_ESC))
	{
		HandleQuitRequested();
	}

	if (g_theInput->WasKeyJustPressed(KEYCODE_F8))
	{
		delete m_theGame;
		m_theGame = new Game();
	}

	if (g_theInput->WasKeyJustPressed('P'))
	{
		m_gameClock.TogglePaused();
	}

	if (g_theInput->WasKeyJustPressed('O'))
	{
		m_gameClock.SetPaused(true);
		m_gameClock.Step();
	}

	m_gameClock.SetTimeScale(g_theInput->IsKeyDown('T') ? 0.1f : 1.f);
}
