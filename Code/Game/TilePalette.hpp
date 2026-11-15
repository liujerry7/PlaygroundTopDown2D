#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Game/TileDefinition.hpp"

class Camera;
class TileMap;
struct AABB2;
struct Vec2;

// Edit-mode tool: a row of tile swatches in screen space, and painting with the mouse in world space.
//   1-9 or click a swatch  pick a brush
//   LMB / RMB              paint / erase (back to the default floor)
//   F                      flood-fill the hovered region with the brush
//   Q                      eyedropper: pick up the hovered tile
class TilePalette
{
public:
	void Update(Camera const& worldCamera, Camera const& screenCamera, TileMap& map);
	void RenderWorldOverlay(TileMap const& map) const; // call inside the world camera
	void RenderScreenOverlay() const;                  // call inside the screen camera

	TileType GetBrushType() const;

private:
	AABB2 GetSwatchBounds(int typeIndex) const;
	AABB2 GetPanelBounds() const;
	int   GetSwatchIndexAt(Vec2 const& screenPos) const; // -1 if none

private:
	TileType m_brushType          = TILE_STONE;
	IntVec2  m_hoveredCoords;
	bool     m_isHoveringPanel    = false;
	int      m_hoveredSwatchIndex = -1;
};
