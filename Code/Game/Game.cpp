#include "Game/Game.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/GameCommon.hpp"

Game::Game()
{
	m_worldCamera.SetOrthoView(Vec2::ZERO, Vec2(WORLD_SIZE_X, WORLD_SIZE_Y));
	m_screenCamera.SetOrthoView(Vec2::ZERO, Vec2(SCREEN_SIZE_X, SCREEN_SIZE_Y));
	m_player.m_position = Vec2(WORLD_SIZE_X / 2.f, WORLD_SIZE_Y / 2.f);
}

Game::~Game()
{
}

void Game::Update(float deltaSeconds)
{
	m_player.Update(deltaSeconds);
}

void Game::Render() const
{
	g_theRenderer->ClearScreen(Rgba8(30, 30, 40));
	g_theRenderer->BeginCameraView(m_worldCamera);
	RenderWorld();
	g_theRenderer->EndCameraView();

	g_theRenderer->BeginCameraView(m_screenCamera);
	RenderUI();
	g_theRenderer->EndCameraView();
}

void Game::RenderWorld() const
{
	m_player.Render();
}

void Game::RenderUI() const
{
}
