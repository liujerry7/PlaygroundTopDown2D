#include "Game/Tank.hpp"
#include <vector>
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Geometry/Disc2.hpp"
#include "Engine/Geometry/OBB2.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/GameCommon.hpp"

static Vec2 const  HULL_HALF_DIMS    = Vec2(0.42f, 0.32f);
static Vec2 const  TREAD_HALF_DIMS   = Vec2(0.46f, 0.09f);
static float const BARREL_LENGTH     = 0.55f;
static float const BARREL_HALF_WIDTH = 0.07f;

// 8-way direction from four keys, or zero if none (or opposing keys) are held.
static Vec2 GetDirectionFromKeys(unsigned char upKey, unsigned char leftKey, unsigned char downKey, unsigned char rightKey)
{
	InputSystem const* input = g_engine->m_inputSystem;

	Vec2 direction;
	if (input->IsKeyDown(upKey))    direction.y += 1.f;
	if (input->IsKeyDown(downKey))  direction.y -= 1.f;
	if (input->IsKeyDown(leftKey))  direction.x -= 1.f;
	if (input->IsKeyDown(rightKey)) direction.x += 1.f;
	return direction;
}

Tank::Tank(Vec2 const& position)
{
	m_body.m_position             = position;
	m_body.m_maxSpeed             = TANK_MAX_SPEED;
	m_body.m_physicsRadius        = TANK_PHYSICS_RADIUS;
	m_body.m_rotation             = TANK_TURN_SPEED;
	m_body.m_hasTargetOrientation = true;
}

void Tank::Update(float deltaSeconds)
{
	UpdateHull();
	UpdateTurret(deltaSeconds);
	UpdateFiring(deltaSeconds);
	m_body.Update(deltaSeconds);
}

// ESDF picks a heading; the hull turns toward it and drives forward while a key is held.
void Tank::UpdateHull()
{
	Vec2 driveDirection = GetDirectionFromKeys('E', 'S', 'D', 'F');
	if (driveDirection == Vec2::ZERO)
	{
		m_body.m_acceleration = Vec2::ZERO;
		return;
	}

	m_body.m_targetOrientation = driveDirection.GetOrientationDegrees();
	m_body.m_acceleration      = Vec2::MakeFromPolarDegrees(m_body.m_orientation, TANK_ACCELERATION);
}

// IJKL aims the turret; it keeps its aim while the hull turns underneath.
void Tank::UpdateTurret(float deltaSeconds)
{
	Vec2 aimDirection = GetDirectionFromKeys('I', 'J', 'K', 'L');
	if (aimDirection == Vec2::ZERO)
	{
		return;
	}

	float goalDegrees   = aimDirection.GetOrientationDegrees();
	m_turretOrientation = GetTurnedTowardDegrees(m_turretOrientation, goalDegrees, TANK_TURRET_TURN_SPEED * deltaSeconds);
}

void Tank::UpdateFiring(float deltaSeconds)
{
	m_fireCooldown -= deltaSeconds;
	if (g_engine->m_inputSystem->IsKeyDown(KEYCODE_SPACE) && m_fireCooldown <= 0.f)
	{
		m_hasPendingShot = true;
		m_fireCooldown   = TANK_FIRE_COOLDOWN;
	}
}

bool Tank::ConsumeShot()
{
	bool hadShot     = m_hasPendingShot;
	m_hasPendingShot = false;
	return hadShot;
}

Vec2 Tank::GetMuzzlePos() const
{
	return m_body.m_position + Vec2::MakeFromPolarDegrees(m_turretOrientation, BARREL_LENGTH);
}

float Tank::GetTurretOrientation() const
{
	return m_turretOrientation;
}

void Tank::Render() const
{
	Vec2 const& center      = m_body.m_position;
	float       hullDegrees = m_body.m_orientation;
	Vec2        left        = Vec2::MakeFromPolarDegrees(hullDegrees + 90.f, HULL_HALF_DIMS.y);

	std::vector<Vertex> verts;
	AddVertsForOBB2D(verts, OBB2(center + left, hullDegrees, TREAD_HALF_DIMS), TANK_TREAD_COLOR);
	AddVertsForOBB2D(verts, OBB2(center - left, hullDegrees, TREAD_HALF_DIMS), TANK_TREAD_COLOR);
	AddVertsForOBB2D(verts, OBB2(center, hullDegrees, HULL_HALF_DIMS), TANK_HULL_COLOR);

	Vec2 barrelCenter = center + Vec2::MakeFromPolarDegrees(m_turretOrientation, 0.5f * BARREL_LENGTH);
	AddVertsForOBB2D(verts, OBB2(barrelCenter, m_turretOrientation, Vec2(0.5f * BARREL_LENGTH, BARREL_HALF_WIDTH)), TANK_TURRET_COLOR);
	AddVertsForDisc2D(verts, Disc2(center, 0.2f), TANK_TURRET_COLOR);

	g_engine->m_renderer->BindTexture(nullptr);
	g_engine->m_renderer->DrawVertexArray(verts);
}
