#pragma once
#include <string>
#include <vector>
#include "Engine/Core/Vertex.hpp"
#include "Engine/Grid/Grid2D.hpp"
#include "Game/TileDefinition.hpp"

struct AABB2;

// Just the grid of tiles. Knows nothing about tanks or bullets; World coordinates them.
class TileMap
{
public:
	explicit TileMap(IntVec2 const& dims);

	static TileMap* CreateDefault();
	static TileMap* CreateFromFile(std::string const& filePath); // nullptr if the file is missing or malformed
	bool            SaveToFile(std::string const& filePath) const;

	IntVec2 GetDimensions() const;
	Vec2    GetSize() const;
	bool    IsInBounds(IntVec2 const& coords) const;
	IntVec2 GetCoordsAtPos(Vec2 const& pos) const; // not clamped; may be out of bounds
	Vec2    GetTileCenter(IntVec2 const& coords) const;
	AABB2   GetTileBounds(IntVec2 const& coords) const;

	// Out-of-bounds tiles read as stone, so the map edge is always a wall.
	TileType              GetTileType(IntVec2 const& coords) const;
	TileDefinition const& GetTileDefinition(IntVec2 const& coords) const;
	void                  SetTileType(IntVec2 const& coords, TileType type);
	void                  FloodFill(IntVec2 const& startCoords, TileType newType);

	// Nearest tile center (by ring search) that a tank can stand on.
	Vec2 FindNearestOpenPos(Vec2 const& pos) const;

	void Render() const;

private:
	void RebuildVerts() const;

private:
	Grid2D                      m_grid;
	std::vector<TileType>       m_tiles;
	mutable std::vector<Vertex> m_verts;
	mutable bool                m_areVertsDirty = true;
};
