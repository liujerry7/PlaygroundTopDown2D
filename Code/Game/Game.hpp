#pragma once
#include "Engine/Renderer/Camera.hpp"

class Game
{
public:
	Game();
	~Game();

	void Update(float deltaSeconds);
	void Render() const;

private:
	void RenderWorld() const;
	void RenderUI() const;

private:
	Camera m_worldCamera;
	Camera m_screenCamera;
};
