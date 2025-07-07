#pragma once 


constexpr float PLAYER_RADIUS = 0.35f;
constexpr float PLAYER_MOVE_SPEED = 3.5f;
constexpr float PLAYER_JUMP_IMPULSE = 2.5f;
constexpr float PLAYER_SPIN_RATE = (180.f / 3.14159265f) / PLAYER_RADIUS;
constexpr float PPM = 100.f;

constexpr float TILE_SIZE = 192.f;

constexpr float WINDOW_WIDTH = 1400.f;
constexpr float WINDOW_HEIGHT = 900.f;

constexpr float ENEMY_WIDTH = 1.0f;
constexpr float ENEMY_HEIGHT = 1.0f;

static constexpr float BOX_DENSITY = 0.2f;      // كثافة 
static constexpr float BOX_FRICTION = 0.4f;     // احتكاك 
static constexpr float BOX_RESTITUTION = 0.05f; // ارتداد 
static constexpr float BOX_SIZE = 180.0f;
