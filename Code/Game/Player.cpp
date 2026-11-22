#include "Game/Player.hpp"
#include <vector>
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Geometry/AABB2.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/MathUtils.hpp"

Player::Player()
{
	AddVertsForAABB2D(m_localVerts, AABB2(-1.f, -1.f, 1.f, 1.f), Rgba8::BLUE);
}

void Player::Update(float deltaSeconds) 
{
	Body::Update(deltaSeconds);
}

void Player::Render() const
{
	std::vector<Vertex> verts = m_localVerts;
	TransformVerticesXY3D(verts, 1.f, m_orientationDegrees, m_position);
	g_engine->m_renderer->DrawVertexArray(verts);
}
