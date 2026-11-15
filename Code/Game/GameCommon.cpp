#include "Game/GameCommon.hpp"
#include <vector>
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Geometry/AABB2.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"

Vec2 GetCursorPosInCamera(Camera const& camera)
{
	Vec2  cursorUV = g_engine->m_inputSystem->GetCursorNormalizedPosition();
	AABB2 view(camera.GetOrthoBottomLeft(), camera.GetOrthoTopRight());
	return view.GetPointAtUV(cursorUV);
}

void DrawBox(AABB2 const& box, Rgba8 const& color)
{
	std::vector<Vertex> verts;
	AddVertsForAABB2D(verts, box, color);
	g_engine->m_renderer->DrawVertexArray(verts);
}

void DrawBoxOutline(AABB2 const& box, float thickness, Rgba8 const& color)
{
	Vec2 const& mins = box.m_mins;
	Vec2 const& maxs = box.m_maxs;

	std::vector<Vertex> verts;
	AddVertsForAABB2D(verts, AABB2(mins.x, mins.y, maxs.x, mins.y + thickness), color);
	AddVertsForAABB2D(verts, AABB2(mins.x, maxs.y - thickness, maxs.x, maxs.y), color);
	AddVertsForAABB2D(verts, AABB2(mins.x, mins.y, mins.x + thickness, maxs.y), color);
	AddVertsForAABB2D(verts, AABB2(maxs.x - thickness, mins.y, maxs.x, maxs.y), color);
	g_engine->m_renderer->DrawVertexArray(verts);
}
