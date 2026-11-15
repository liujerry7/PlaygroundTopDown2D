#pragma once
#include <vector>
#include "Engine/Math/Vec2.hpp"

class Tank;
class TileMap;

struct Bullet
{
	Vec2  m_position;
	Vec2  m_velocity;
	int   m_bouncesLeft = 0;
	float m_age         = 0.f;
	bool  m_isDead      = false;
};

// Owns the tile map and the entities and coordinates them. Talks to entities only through their bodies.
class World
{
public:
	explicit World(TileMap* map); // takes ownership
	~World();

	void Update(float deltaSeconds);
	void Render() const;

	TileMap&    GetMap();
	Tank const& GetTank() const;
	void        ReplaceMap(TileMap* newMap); // takes ownership; moves the tank somewhere open

private:
	void UpdateBullets(float deltaSeconds);
	void MoveBullet(Bullet& bullet, float deltaSeconds);
	void ResolveTankAgainstTiles();
	void ApplyTileEffects(float deltaSeconds);
	void FireBullet(Vec2 const& position, float orientationDegrees);

private:
	TileMap*            m_map  = nullptr;
	Tank*               m_tank = nullptr;
	std::vector<Bullet> m_bullets;
};
