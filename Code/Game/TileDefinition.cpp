#include "Game/TileDefinition.hpp"

// Indexed by TileType. Tweak these to experiment; the palette picks up new rows automatically.
//                                     name     glyph  color                  moves  bullets  breaks  drag
static TileDefinition const s_tileDefinitions[NUM_TILE_TYPES] = {
	{ "Grass", '.', Rgba8(70, 120, 60),   false, false, false, 3.f  },
	{ "Stone", '#', Rgba8(110, 110, 120), true,  true,  false, 3.f  },
	{ "Brick", 'B', Rgba8(160, 80, 60),   true,  true,  true,  3.f  },
	{ "Mud",   'm', Rgba8(100, 75, 45),   false, false, false, 14.f },
	{ "Ice",   'i', Rgba8(170, 220, 240), false, false, false, 0.3f },
	{ "Sand",  's', Rgba8(210, 190, 120), false, false, false, 7.f  },
	{ "Water", '~', Rgba8(40, 90, 170),   true,  false, false, 3.f  },
};

TileDefinition const& GetTileDefinition(TileType type)
{
	return s_tileDefinitions[type];
}

bool GetTileTypeForGlyph(char glyph, TileType& outType)
{
	for (int typeIndex = 0; typeIndex < NUM_TILE_TYPES; ++typeIndex)
	{
		if (s_tileDefinitions[typeIndex].m_glyph == glyph)
		{
			outType = static_cast<TileType>(typeIndex);
			return true;
		}
	}

	return false;
}
