#include "Game/Game.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Tank.hpp"
#include "Game/TileMap.hpp"
#include "Game/World.hpp"

static float const STATUS_DURATION_SECONDS = 2.5f;
static float const PALETTE_SCREEN_FRACTION = 0.13f; // bottom strip of the screen the palette covers in edit mode
static Rgba8 const BACKGROUND_COLOR        = Rgba8(20, 20, 28);

Game::Game()
{
	m_screenCamera.SetOrthoView(Vec2::ZERO, Vec2(SCREEN_SIZE_X, SCREEN_SIZE_Y));

	TileMap* map = TileMap::CreateFromFile(MAP_FILE_PATH);
	if (!map)
	{
		map = TileMap::CreateDefault();
	}

	m_world = new World(map);
	FitCamerasToMap();
}

Game::~Game()
{
	delete m_world;
	m_world = nullptr;
}

void Game::Update(float deltaSeconds)
{
	HandleModeKeys();

	if (m_isEditing)
	{
		m_palette.Update(m_editCamera, m_screenCamera, m_world->GetMap());
	}
	else
	{
		m_world->Update(deltaSeconds);
		UpdatePlayCamera(deltaSeconds);
	}

	UpdateWindowTitle(deltaSeconds);
}

void Game::Render() const
{
	Renderer* renderer = g_engine->m_renderer;
	renderer->ClearScreen(BACKGROUND_COLOR);

	Camera const& worldCamera = m_isEditing ? m_editCamera : m_playCamera;
	renderer->BeginCameraView(worldCamera);
	m_world->Render();
	if (m_isEditing)
	{
		m_palette.RenderWorldOverlay(m_world->GetMap());
	}
	renderer->EndCameraView();

	if (m_isEditing)
	{
		renderer->BeginCameraView(m_screenCamera);
		m_palette.RenderScreenOverlay();
		renderer->EndCameraView();
	}
}

void Game::HandleModeKeys()
{
	InputSystem const* input = g_engine->m_inputSystem;

	if (input->WasKeyJustPressed(KEYCODE_TAB))
	{
		SetEditing(!m_isEditing);
	}

	if (input->WasKeyJustPressed(KEYCODE_F6))
	{
		SaveMap();
	}

	if (input->WasKeyJustPressed(KEYCODE_F9))
	{
		LoadMap();
	}
}

void Game::SaveMap()
{
	bool didSave = m_world->GetMap().SaveToFile(MAP_FILE_PATH);
	ShowStatus(didSave ? Stringf("Saved %s", MAP_FILE_PATH) : Stringf("Couldn't save %s", MAP_FILE_PATH));
}

void Game::LoadMap()
{
	TileMap* map = TileMap::CreateFromFile(MAP_FILE_PATH);
	if (!map)
	{
		ShowStatus(Stringf("Couldn't load %s", MAP_FILE_PATH));
		return;
	}

	m_world->ReplaceMap(map);
	FitCamerasToMap();
	ShowStatus(Stringf("Loaded %s", MAP_FILE_PATH));
}

void Game::SetEditing(bool isEditing)
{
	m_isEditing = isEditing;
}

// Edit camera shows the whole map (letterboxed to the screen aspect); play camera follows the tank,
// clamped to the map, and shrinks if the map is smaller than its usual view.
void Game::FitCamerasToMap()
{
	Vec2  mapSize      = m_world->GetMap().GetSize();
	float screenAspect = SCREEN_SIZE_X / SCREEN_SIZE_Y;

	// The map is centered in the part of the screen above the palette.
	float usableFraction = 1.f - PALETTE_SCREEN_FRACTION;
	float editHeight     = GetMax(mapSize.y, mapSize.x / screenAspect * usableFraction) / usableFraction;
	Vec2  editSize       = Vec2(editHeight * screenAspect, editHeight);
	Vec2  mapCenter      = 0.5f * mapSize;
	Vec2  editMins       = Vec2(mapCenter.x - 0.5f * editSize.x, mapCenter.y - 0.5f * editHeight * usableFraction - editHeight * PALETTE_SCREEN_FRACTION);
	m_editCamera.SetOrthoView(editMins, editMins + editSize);

	float playHeight = GetMin(PLAY_VIEW_TILES_Y, GetMin(mapSize.y, mapSize.x / screenAspect));
	Vec2  playSize   = Vec2(playHeight * screenAspect, playHeight);
	m_playCamera.SetOrthoView(Vec2::ZERO, playSize);
	m_playCamera.SetMovementBounds(Vec2::ZERO, mapSize);
	m_playCamera.SetCenter(m_world->GetTank().m_body.m_position);
}

void Game::UpdatePlayCamera(float deltaSeconds)
{
	m_playCamera.SetFollowTarget(m_world->GetTank().m_body.m_position);
	m_playCamera.Update(deltaSeconds);
}

// No text rendering in Jumble yet, so the window title carries the mode, brush and status messages.
void Game::UpdateWindowTitle(float deltaSeconds)
{
	m_statusSecondsLeft -= deltaSeconds;

	std::string title;
	if (m_isEditing)
	{
		TileType brush = m_palette.GetBrushType();
		title = Stringf("Libra  |  EDIT  |  Brush: %s (%d)  |  LMB paint, RMB erase, F fill, Q pick  |  F6 save, F9 load  |  Tab: play",
			GetTileDefinition(brush).m_name, static_cast<int>(brush) + 1);
	}
	else
	{
		title = "Libra  |  PLAY  |  ESDF drive, IJKL aim, Space fire  |  F6 save, F9 load  |  Tab: edit";
	}

	if (m_statusSecondsLeft > 0.f)
	{
		title += "  |  " + m_statusMessage;
	}

	if (title != m_windowTitle)
	{
		m_windowTitle = title;
		g_engine->m_window->SetTitle(title);
	}
}

void Game::ShowStatus(std::string const& message)
{
	m_statusMessage     = message;
	m_statusSecondsLeft = STATUS_DURATION_SECONDS;
}
