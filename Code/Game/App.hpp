#pragma once
#include "Engine/Core/Clock.hpp"

class Game;

class App
{
public:
	App();
	~App();

	void Startup();
	void Shutdown();
	void RunMainLoop();
	void RunFrame();

	bool IsQuitting() const;
	void HandleQuitRequested();

private:
	void BeginFrame();
	void Update();
	void Render() const;
	void EndFrame();

	void HandleDeveloperKeys();

private:
	Game* m_theGame    = nullptr;
	Clock m_gameClock;
	bool  m_isQuitting = false;
};
