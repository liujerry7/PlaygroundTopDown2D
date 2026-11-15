#pragma once
#include <string>
#include "Engine/Renderer/Camera.hpp"
#include "Game/TilePalette.hpp"

class World;

// Switches between play mode (drive the tank) and edit mode (paint tiles), and owns the cameras.
//   Tab      toggle play / edit
//   F6 / F9  save / load Data/Maps/Playground.txt
class Game
{
public:
	Game();
	~Game();

	void Update(float deltaSeconds);
	void Render() const;

private:
	void HandleModeKeys();
	void SaveMap();
	void LoadMap();
	void SetEditing(bool isEditing);
	void FitCamerasToMap();
	void UpdatePlayCamera(float deltaSeconds);
	void UpdateWindowTitle(float deltaSeconds);
	void ShowStatus(std::string const& message);

private:
	World*      m_world     = nullptr;
	TilePalette m_palette;
	Camera      m_playCamera;
	Camera      m_editCamera;
	Camera      m_screenCamera;
	bool        m_isEditing = false;

	std::string m_statusMessage;
	float       m_statusSecondsLeft = 0.f;
	std::string m_windowTitle;
};
