#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"

class Camera;
struct AABB2;

// Screen space (UI) is a fixed 1600x900 canvas; world space is measured in tiles.
constexpr float SCREEN_SIZE_X = 1600.f;
constexpr float SCREEN_SIZE_Y = 900.f;

constexpr float PLAY_VIEW_TILES_Y = 13.5f; // play camera height; width follows the screen aspect

// Jumble only names a few keys; letters are their ASCII codes.
constexpr unsigned char KEYCODE_TAB = 0x09;

char const* const MAP_FILE_PATH = "Data/Maps/Playground.txt";

// Tank
constexpr float TANK_ACCELERATION      = 24.f;
constexpr float TANK_MAX_SPEED         = 9.f;
constexpr float TANK_TURN_SPEED        = 220.f;
constexpr float TANK_TURRET_TURN_SPEED = 360.f;
constexpr float TANK_PHYSICS_RADIUS    = 0.35f;
constexpr float TANK_FIRE_COOLDOWN     = 0.2f;
constexpr float TANK_DRAG_BLEND_SPEED  = 8.f; // how fast the tank's drag follows the tile under it

// Bullets
constexpr float BULLET_SPEED        = 12.f;
constexpr float BULLET_RADIUS       = 0.08f;
constexpr int   BULLET_MAX_BOUNCES  = 3;
constexpr float BULLET_MAX_LIFETIME = 4.f;

Rgba8 const TANK_HULL_COLOR   = Rgba8(70, 110, 190);
Rgba8 const TANK_TREAD_COLOR  = Rgba8(40, 50, 70);
Rgba8 const TANK_TURRET_COLOR = Rgba8(150, 190, 255);
Rgba8 const BULLET_COLOR      = Rgba8(255, 230, 120);

// Cursor position in the given camera's view, from the OS cursor.
Vec2 GetCursorPosInCamera(Camera const& camera);

void DrawBox(AABB2 const& box, Rgba8 const& color);
void DrawBoxOutline(AABB2 const& box, float thickness, Rgba8 const& color);
