#pragma once

class App;
class AudioSystem;
class InputSystem;
class Renderer;
class Window;

// Shortcuts to the engine's subsystems, set by App::Startup. g_engine owns them.
extern App*         g_theApp;
extern Renderer*    g_theRenderer;
extern InputSystem* g_theInput;
extern AudioSystem* g_theAudio;
extern Window*      g_theWindow;

constexpr float WORLD_SIZE_X  = 200.f;
constexpr float WORLD_SIZE_Y  = 100.f;
constexpr float SCREEN_SIZE_X = 1600.f;
constexpr float SCREEN_SIZE_Y = 800.f;
