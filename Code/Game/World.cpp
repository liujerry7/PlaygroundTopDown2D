#include "Game/World.hpp"
#include <algorithm>
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Geometry/AABB2.hpp"
#include "Engine/Geometry/Disc2.hpp"
#include "Engine/Geometry/GeometryUtils.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Tank.hpp"
#include "Game/TileMap.hpp"

static float const BULLET_MAX_STEP = 0.1f; // bullets move in steps this long so they can't skip a tile

// Orthogonal neighbors first, so the tank slides along flat walls before corners get a say.
static IntVec2 const NEIGHBOR_OFFSETS[8] = {
	IntVec2(1, 0),  IntVec2(-1, 0), IntVec2(0, 1),  IntVec2(0, -1),
	IntVec2(1, 1),  IntVec2(-1, 1), IntVec2(1, -1), IntVec2(-1, -1),
};

World::World(TileMap* map)
	: m_map(map)
{
	Vec2 spawnPos         = m_map->FindNearestOpenPos(0.5f * m_map->GetSize());
	m_tank                = new Tank(spawnPos);
	m_tank->m_body.m_drag = m_map->GetTileDefinition(m_map->GetCoordsAtPos(spawnPos)).m_drag;
}

World::~World()
{
	delete m_tank;
	m_tank = nullptr;

	delete m_map;
	m_map = nullptr;
}

// Frame order: entities move by their own rules, then the world resolves them against the tiles.
void World::Update(float deltaSeconds)
{
	m_tank->Update(deltaSeconds);
	if (m_tank->ConsumeShot())
	{
		FireBullet(m_tank->GetMuzzlePos(), m_tank->GetTurretOrientation());
	}

	UpdateBullets(deltaSeconds);
	ResolveTankAgainstTiles();
	ApplyTileEffects(deltaSeconds);
}

void World::Render() const
{
	m_map->Render();

	std::vector<Vertex> bulletVerts;
	for (Bullet const& bullet : m_bullets)
	{
		AddVertsForDisc2D(bulletVerts, Disc2(bullet.m_position, BULLET_RADIUS), BULLET_COLOR);
	}
	g_engine->m_renderer->DrawVertexArray(bulletVerts);

	m_tank->Render();
}

TileMap& World::GetMap()
{
	return *m_map;
}

Tank const& World::GetTank() const
{
	return *m_tank;
}

void World::ReplaceMap(TileMap* newMap)
{
	delete m_map;
	m_map = newMap;

	m_bullets.clear();
	m_tank->m_body.m_position = m_map->FindNearestOpenPos(m_tank->m_body.m_position);
	m_tank->m_body.m_velocity = Vec2::ZERO;
}

void World::UpdateBullets(float deltaSeconds)
{
	for (Bullet& bullet : m_bullets)
	{
		bullet.m_age += deltaSeconds;
		if (bullet.m_age > BULLET_MAX_LIFETIME)
		{
			bullet.m_isDead = true;
			continue;
		}

		MoveBullet(bullet, deltaSeconds);
	}

	auto isDead = [](Bullet const& bullet) { return bullet.m_isDead; };
	m_bullets.erase(std::remove_if(m_bullets.begin(), m_bullets.end(), isDead), m_bullets.end());
}

// Bullets bounce off tiles that block them, flipping whichever velocity axis crossed into the tile.
// Destructible tiles break instead, and the bullet dies with them.
void World::MoveBullet(Bullet& bullet, float deltaSeconds)
{
	Vec2  displacement = bullet.m_velocity * deltaSeconds;
	int   numSteps     = 1 + static_cast<int>(displacement.GetLength() / BULLET_MAX_STEP);
	float stepSeconds  = deltaSeconds / static_cast<float>(numSteps);

	for (int stepIndex = 0; stepIndex < numSteps; ++stepIndex)
	{
		IntVec2 prevCoords = m_map->GetCoordsAtPos(bullet.m_position);
		Vec2    nextPos    = bullet.m_position + bullet.m_velocity * stepSeconds;
		IntVec2 nextCoords = m_map->GetCoordsAtPos(nextPos);

		TileDefinition const& nextTile = m_map->GetTileDefinition(nextCoords);
		if (nextCoords == prevCoords || !nextTile.m_blocksBullets)
		{
			bullet.m_position = nextPos;
			continue;
		}

		if (nextTile.m_isDestructible)
		{
			m_map->SetTileType(nextCoords, DEFAULT_FLOOR_TILE);
			bullet.m_isDead = true;
			return;
		}

		if (bullet.m_bouncesLeft <= 0)
		{
			bullet.m_isDead = true;
			return;
		}
		bullet.m_bouncesLeft--;

		bool crossedX = nextCoords.x != prevCoords.x;
		bool crossedY = nextCoords.y != prevCoords.y;
		if (crossedX && crossedY)
		{
			// Diagonal move: flip only the axes whose side tile is actually in the way.
			bool blockedX = m_map->GetTileDefinition(IntVec2(nextCoords.x, prevCoords.y)).m_blocksBullets;
			bool blockedY = m_map->GetTileDefinition(IntVec2(prevCoords.x, nextCoords.y)).m_blocksBullets;
			if (blockedX == blockedY)
			{
				blockedX = true;
				blockedY = true;
			}
			crossedX = blockedX;
			crossedY = blockedY;
		}

		if (crossedX)
		{
			bullet.m_velocity.x = -bullet.m_velocity.x;
		}
		if (crossedY)
		{
			bullet.m_velocity.y = -bullet.m_velocity.y;
		}
	}
}

void World::ResolveTankAgainstTiles()
{
	Body&   body   = m_tank->m_body;
	IntVec2 coords = m_map->GetCoordsAtPos(body.m_position);

	// Painted over (or spawned inside) a wall: hop to the nearest open tile.
	if (m_map->GetTileDefinition(coords).m_blocksMovement)
	{
		body.m_position = m_map->FindNearestOpenPos(body.m_position);
		body.m_velocity = Vec2::ZERO;
		return;
	}

	for (IntVec2 const& offset : NEIGHBOR_OFFSETS)
	{
		IntVec2 neighborCoords = coords + offset;
		if (!m_map->GetTileDefinition(neighborCoords).m_blocksMovement)
		{
			continue;
		}

		Vec2 posBefore = body.m_position;
		if (!PushDiscOutOfFixedAABB2D(body.m_position, body.m_physicsRadius, m_map->GetTileBounds(neighborCoords)))
		{
			continue;
		}

		// Drop the part of the velocity driving into the wall, so the tank slides along it (nice on ice).
		Vec2  pushNormal    = (body.m_position - posBefore).GetNormalized();
		float speedIntoWall = DotProduct2D(body.m_velocity, pushNormal);
		if (speedIntoWall < 0.f)
		{
			body.m_velocity -= pushNormal * speedIntoWall;
		}
	}
}

// The tile under the tank's center sets a target drag, and the body's drag eases toward it.
void World::ApplyTileEffects(float deltaSeconds)
{
	Body&                 body = m_tank->m_body;
	TileDefinition const& tile = m_map->GetTileDefinition(m_map->GetCoordsAtPos(body.m_position));

	float blend = GetMin(TANK_DRAG_BLEND_SPEED * deltaSeconds, 1.f);
	body.m_drag = Interpolate(body.m_drag, tile.m_drag, blend);
}

void World::FireBullet(Vec2 const& position, float orientationDegrees)
{
	Bullet bullet;
	bullet.m_position    = position;
	bullet.m_velocity    = Vec2::MakeFromPolarDegrees(orientationDegrees, BULLET_SPEED);
	bullet.m_bouncesLeft = BULLET_MAX_BOUNCES;

	// Muzzle already inside a wall: the shot hits it on the spot.
	IntVec2               coords = m_map->GetCoordsAtPos(position);
	TileDefinition const& tile   = m_map->GetTileDefinition(coords);
	if (tile.m_blocksBullets)
	{
		if (tile.m_isDestructible)
		{
			m_map->SetTileType(coords, DEFAULT_FLOOR_TILE);
		}
		return;
	}

	m_bullets.push_back(bullet);
}
