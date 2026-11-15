#include "Game/App.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"

App* g_app = nullptr;

App::App()
{
	EngineConfig config;
	config.m_windowConfig.m_windowTitle  = "Libra";
	config.m_windowConfig.m_clientAspect = SCREEN_SIZE_X / SCREEN_SIZE_Y;

	new Engine(config); // sets g_engine
	m_game = new Game();
}

App::~App()
{
	delete m_game;
	m_game = nullptr;

	delete g_engine;
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
	return m_isQuitting || g_engine->m_window->m_quitRequested;
}

void App::BeginFrame()
{
	g_engine->BeginFrame();
	m_clock.BeginFrame();
}

void App::Update()
{
	HandleDeveloperKeys();
	m_game->Update(m_clock.GetDeltaSeconds());
}

void App::Render() const
{
	m_game->Render();
}

void App::EndFrame()
{
	m_clock.EndFrame();
	g_engine->EndFrame();
}

// Esc quits, F8 rebuilds the Game, P pauses, O steps one frame, T (held) runs at 10% speed.
void App::HandleDeveloperKeys()
{
	InputSystem const* input = g_engine->m_inputSystem;

	if (input->WasKeyJustPressed(KEYCODE_ESC))
	{
		m_isQuitting = true;
	}

	if (input->WasKeyJustPressed(KEYCODE_F8))
	{
		delete m_game;
		m_game = new Game();
	}

	if (input->WasKeyJustPressed('P'))
	{
		m_clock.TogglePaused();
	}

	if (input->WasKeyJustPressed('O'))
	{
		m_clock.SetPaused(true);
		m_clock.Step();
	}

	m_clock.SetTimeScale(input->IsKeyDown('T') ? 0.1f : 1.f);
}
