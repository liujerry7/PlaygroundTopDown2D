#pragma once
#include "Engine/Physics/Body.hpp"

// The player's tank. Owns a Body for movement and knows nothing about tiles;
// World pushes it out of walls and applies tile effects through the body.
class Tank
{
public:
	explicit Tank(Vec2 const& position);

	void Update(float deltaSeconds);
	void Render() const;

	bool ConsumeShot(); // true once per shot the player fired this frame
	Vec2 GetMuzzlePos() const;
	float GetTurretOrientation() const;

public:
	Body m_body;

private:
	void UpdateHull();
	void UpdateTurret(float deltaSeconds);
	void UpdateFiring(float deltaSeconds);

private:
	float m_turretOrientation = 0.f; // absolute, not relative to the hull
	float m_fireCooldown      = 0.f;
	bool  m_hasPendingShot    = false;
};
