#include "Game/TileMap.hpp"
#include <filesystem>
#include <fstream>
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Rgba8Utils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Geometry/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"

static IntVec2 const DEFAULT_MAP_DIMS = IntVec2(48, 27);

TileMap::TileMap(IntVec2 const& dims)
	: m_grid(dims)
	, m_tiles(dims.x * dims.y, DEFAULT_FLOOR_TILE)
{
}

// A walled field with a patch of every tile type, so there's something to drive on right away.
TileMap* TileMap::CreateDefault()
{
	TileMap* map  = new TileMap(DEFAULT_MAP_DIMS);
	IntVec2  dims = map->GetDimensions();

	auto fillRect = [map](int minX, int minY, int maxX, int maxY, TileType type)
	{
		for (int y = minY; y <= maxY; ++y)
		{
			for (int x = minX; x <= maxX; ++x)
			{
				map->SetTileType(IntVec2(x, y), type);
			}
		}
	};

	fillRect(0, 0, dims.x - 1, 0, TILE_STONE);
	fillRect(0, dims.y - 1, dims.x - 1, dims.y - 1, TILE_STONE);
	fillRect(0, 0, 0, dims.y - 1, TILE_STONE);
	fillRect(dims.x - 1, 0, dims.x - 1, dims.y - 1, TILE_STONE);

	fillRect(4, 4, 11, 9, TILE_MUD);
	fillRect(30, 4, 43, 10, TILE_ICE);
	fillRect(15, 17, 21, 22, TILE_SAND);
	fillRect(33, 16, 40, 22, TILE_WATER);
	fillRect(18, 8, 19, 14, TILE_STONE);
	fillRect(28, 12, 32, 13, TILE_BRICK);
	fillRect(6, 15, 7, 22, TILE_BRICK);

	return map;
}

// One line per row, top row first; each character is a tile glyph. Unknown glyphs read as floor.
TileMap* TileMap::CreateFromFile(std::string const& filePath)
{
	std::ifstream file(filePath);
	if (!file)
	{
		return nullptr;
	}

	std::vector<std::string> rows;
	std::string              line;
	while (std::getline(file, line))
	{
		if (!line.empty() && line.back() == '\r')
		{
			line.pop_back();
		}

		if (!line.empty())
		{
			rows.push_back(line);
		}
	}

	if (rows.empty())
	{
		return nullptr;
	}

	int width = static_cast<int>(rows[0].size());
	for (std::string const& row : rows)
	{
		if (static_cast<int>(row.size()) != width)
		{
			return nullptr;
		}
	}

	int      height = static_cast<int>(rows.size());
	TileMap* map    = new TileMap(IntVec2(width, height));
	for (int rowIndex = 0; rowIndex < height; ++rowIndex)
	{
		int y = height - 1 - rowIndex;
		for (int x = 0; x < width; ++x)
		{
			TileType type = DEFAULT_FLOOR_TILE;
			GetTileTypeForGlyph(rows[rowIndex][x], type);
			map->SetTileType(IntVec2(x, y), type);
		}
	}

	return map;
}

bool TileMap::SaveToFile(std::string const& filePath) const
{
	std::filesystem::path path(filePath);
	if (path.has_parent_path())
	{
		std::error_code error;
		std::filesystem::create_directories(path.parent_path(), error);
	}

	std::ofstream file(filePath);
	if (!file)
	{
		return false;
	}

	IntVec2 dims = GetDimensions();
	for (int y = dims.y - 1; y >= 0; --y)
	{
		std::string row;
		for (int x = 0; x < dims.x; ++x)
		{
			row += GetTileDefinition(IntVec2(x, y)).m_glyph;
		}
		file << row << '\n';
	}

	return static_cast<bool>(file);
}

IntVec2 TileMap::GetDimensions() const
{
	return m_grid.dims;
}

Vec2 TileMap::GetSize() const
{
	return Vec2(m_grid.dims) * m_grid.cellSize;
}

bool TileMap::IsInBounds(IntVec2 const& coords) const
{
	return m_grid.IsInBounds(coords);
}

IntVec2 TileMap::GetCoordsAtPos(Vec2 const& pos) const
{
	return IntVec2(RoundDownToInt(pos.x / m_grid.cellSize), RoundDownToInt(pos.y / m_grid.cellSize));
}

Vec2 TileMap::GetTileCenter(IntVec2 const& coords) const
{
	return m_grid.GetTileCenter(coords);
}

AABB2 TileMap::GetTileBounds(IntVec2 const& coords) const
{
	Vec2 mins = Vec2(coords) * m_grid.cellSize;
	return AABB2(mins, mins + Vec2(m_grid.cellSize, m_grid.cellSize));
}

TileType TileMap::GetTileType(IntVec2 const& coords) const
{
	if (!IsInBounds(coords))
	{
		return TILE_STONE;
	}

	return m_tiles[m_grid.GetIdx(coords)];
}

TileDefinition const& TileMap::GetTileDefinition(IntVec2 const& coords) const
{
	return ::GetTileDefinition(GetTileType(coords));
}

void TileMap::SetTileType(IntVec2 const& coords, TileType type)
{
	if (!IsInBounds(coords))
	{
		return;
	}

	TileType& tile = m_tiles[m_grid.GetIdx(coords)];
	if (tile != type)
	{
		tile            = type;
		m_areVertsDirty = true;
	}
}

void TileMap::FloodFill(IntVec2 const& startCoords, TileType newType)
{
	if (!IsInBounds(startCoords))
	{
		return;
	}

	TileType oldType = GetTileType(startCoords);
	if (oldType == newType)
	{
		return;
	}

	std::vector<IntVec2> open;
	open.push_back(startCoords);
	while (!open.empty())
	{
		IntVec2 coords = open.back();
		open.pop_back();

		if (!IsInBounds(coords) || GetTileType(coords) != oldType)
		{
			continue;
		}

		SetTileType(coords, newType);
		open.push_back(coords + IntVec2::RIGHT);
		open.push_back(coords + IntVec2::LEFT);
		open.push_back(coords + IntVec2::UP);
		open.push_back(coords + IntVec2::DOWN);
	}
}

Vec2 TileMap::FindNearestOpenPos(Vec2 const& pos) const
{
	IntVec2 center    = m_grid.GetClampedCoords(GetCoordsAtPos(pos));
	int     maxRadius = GetMaxInt(m_grid.dims.x, m_grid.dims.y);

	for (int radius = 0; radius <= maxRadius; ++radius)
	{
		for (int y = center.y - radius; y <= center.y + radius; ++y)
		{
			for (int x = center.x - radius; x <= center.x + radius; ++x)
			{
				IntVec2 coords(x, y);
				bool    isOnRing = (coords - center).GetChebyshevLength() == radius;
				if (isOnRing && IsInBounds(coords) && !GetTileDefinition(coords).m_blocksMovement)
				{
					return GetTileCenter(coords);
				}
			}
		}
	}

	return pos;
}

void TileMap::Render() const
{
	if (m_areVertsDirty)
	{
		RebuildVerts();
	}

	g_engine->m_renderer->BindTexture(nullptr);
	g_engine->m_renderer->DrawVertexArray(m_verts);
}

// Tiles are data, so the map batches every tile into one vertex array and only rebuilds it after an edit.
void TileMap::RebuildVerts() const
{
	m_verts.clear();
	m_verts.reserve(m_tiles.size() * 6);

	IntVec2 dims = GetDimensions();
	for (int y = 0; y < dims.y; ++y)
	{
		for (int x = 0; x < dims.x; ++x)
		{
			IntVec2 coords(x, y);
			Rgba8   color = GetTileDefinition(coords).m_color;

			// Checkerboard shading so single tiles read clearly while editing.
			if ((x + y) % 2 == 1)
			{
				color = InterpolateColor(color, Rgba8::BLACK, 0.08f);
			}

			AddVertsForAABB2D(m_verts, GetTileBounds(coords), color);
		}
	}

	m_areVertsDirty = false;
}
