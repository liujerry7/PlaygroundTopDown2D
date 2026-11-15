#pragma once
#include "Engine/Core/Clock.hpp"

class App;
class Game;

extern App* g_app;

class App
{
public:
	App();
	~App();

	void RunFrame();
	bool IsQuitting() const;

private:
	void BeginFrame();
	void Update();
	void Render() const;
	void EndFrame();
	void HandleDeveloperKeys();

private:
	Game* m_game       = nullptr;
	Clock m_clock;
	bool  m_isQuitting = false;
};
