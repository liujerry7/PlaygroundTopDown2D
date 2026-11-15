#include "Game/TilePalette.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Geometry/AABB2.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Game/GameCommon.hpp"
#include "Game/TileMap.hpp"

static float const SWATCH_SIZE   = 64.f;
static float const SWATCH_GAP    = 12.f;
static float const PANEL_MARGIN  = 16.f;
static float const PANEL_PADDING = 12.f;

void TilePalette::Update(Camera const& worldCamera, Camera const& screenCamera, TileMap& map)
{
	InputSystem const* input = g_engine->m_inputSystem;

	for (int typeIndex = 0; typeIndex < NUM_TILE_TYPES && typeIndex < 9; ++typeIndex)
	{
		if (input->WasKeyJustPressed(static_cast<unsigned char>('1' + typeIndex)))
		{
			m_brushType = static_cast<TileType>(typeIndex);
		}
	}

	Vec2 cursorScreenPos = GetCursorPosInCamera(screenCamera);
	m_isHoveringPanel    = GetPanelBounds().IsPointInside(cursorScreenPos);
	m_hoveredSwatchIndex = GetSwatchIndexAt(cursorScreenPos);
	m_hoveredCoords      = map.GetCoordsAtPos(GetCursorPosInCamera(worldCamera));

	if (m_isHoveringPanel)
	{
		if (input->WasKeyJustPressed(KEYCODE_LMB) && m_hoveredSwatchIndex >= 0)
		{
			m_brushType = static_cast<TileType>(m_hoveredSwatchIndex);
		}
		return;
	}

	if (!map.IsInBounds(m_hoveredCoords))
	{
		return;
	}

	if (input->IsKeyDown(KEYCODE_LMB))
	{
		map.SetTileType(m_hoveredCoords, m_brushType);
	}
	else if (input->IsKeyDown(KEYCODE_RMB))
	{
		map.SetTileType(m_hoveredCoords, DEFAULT_FLOOR_TILE);
	}

	if (input->WasKeyJustPressed('F'))
	{
		map.FloodFill(m_hoveredCoords, m_brushType);
	}

	if (input->WasKeyJustPressed('Q'))
	{
		m_brushType = map.GetTileType(m_hoveredCoords);
	}
}

void TilePalette::RenderWorldOverlay(TileMap const& map) const
{
	if (m_isHoveringPanel || !map.IsInBounds(m_hoveredCoords))
	{
		return;
	}

	AABB2 tileBounds = map.GetTileBounds(m_hoveredCoords);
	DrawBox(tileBounds, Rgba8(255, 255, 255, 40));
	DrawBoxOutline(tileBounds, 0.06f, Rgba8::WHITE);
}

void TilePalette::RenderScreenOverlay() const
{
	DrawBox(GetPanelBounds(), Rgba8(0, 0, 0, 170));

	for (int typeIndex = 0; typeIndex < NUM_TILE_TYPES; ++typeIndex)
	{
		AABB2 swatch = GetSwatchBounds(typeIndex);
		DrawBox(swatch, GetTileDefinition(static_cast<TileType>(typeIndex)).m_color);

		if (typeIndex == m_brushType)
		{
			AABB2 ring = swatch;
			ring.SetDimensions(swatch.GetDimensions() + Vec2(10.f, 10.f));
			DrawBoxOutline(ring, 4.f, Rgba8::WHITE);
		}
		else if (typeIndex == m_hoveredSwatchIndex)
		{
			DrawBoxOutline(swatch, 2.f, Rgba8(255, 255, 255, 140));
		}
	}

	// Yellow frame around the screen so edit mode is obvious at a glance.
	DrawBoxOutline(AABB2(0.f, 0.f, SCREEN_SIZE_X, SCREEN_SIZE_Y), 6.f, Rgba8(255, 210, 60));
}

TileType TilePalette::GetBrushType() const
{
	return m_brushType;
}

AABB2 TilePalette::GetSwatchBounds(int typeIndex) const
{
	float minX = PANEL_MARGIN + PANEL_PADDING + static_cast<float>(typeIndex) * (SWATCH_SIZE + SWATCH_GAP);
	float minY = PANEL_MARGIN + PANEL_PADDING;
	return AABB2(minX, minY, minX + SWATCH_SIZE, minY + SWATCH_SIZE);
}

AABB2 TilePalette::GetPanelBounds() const
{
	float width  = 2.f * PANEL_PADDING + NUM_TILE_TYPES * SWATCH_SIZE + (NUM_TILE_TYPES - 1) * SWATCH_GAP;
	float height = 2.f * PANEL_PADDING + SWATCH_SIZE;
	return AABB2(PANEL_MARGIN, PANEL_MARGIN, PANEL_MARGIN + width, PANEL_MARGIN + height);
}

int TilePalette::GetSwatchIndexAt(Vec2 const& screenPos) const
{
	for (int typeIndex = 0; typeIndex < NUM_TILE_TYPES; ++typeIndex)
	{
		if (GetSwatchBounds(typeIndex).IsPointInside(screenPos))
		{
			return typeIndex;
		}
	}

	return -1;
}
