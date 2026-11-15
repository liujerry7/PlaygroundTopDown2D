#pragma once
#include "Engine/Core/Rgba8.hpp"

// A tile stores only its type; everything else lives in the shared definition for that type.
enum TileType : unsigned char
{
	TILE_GRASS,
	TILE_STONE,
	TILE_BRICK,
	TILE_MUD,
	TILE_ICE,
	TILE_SAND,
	TILE_WATER,
	NUM_TILE_TYPES
};

constexpr TileType DEFAULT_FLOOR_TILE = TILE_GRASS;

struct TileDefinition
{
	char const* m_name             = "";
	char        m_glyph            = '.';   // character used in map files
	Rgba8       m_color;
	bool        m_blocksMovement   = false; // tanks can't enter
	bool        m_blocksBullets    = false; // bullets bounce off (or break it, if destructible)
	bool        m_isDestructible   = false; // a bullet turns it into the default floor
	float       m_drag             = 3.f;   // drag on a body whose center is over this tile
};

TileDefinition const& GetTileDefinition(TileType type);
bool                  GetTileTypeForGlyph(char glyph, TileType& outType);
