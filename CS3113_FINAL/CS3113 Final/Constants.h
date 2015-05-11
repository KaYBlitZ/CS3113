#pragma once

namespace Constants {
	const float DEG_TO_RAD = 0.0174532925f;
	const float MOUSE_SENSITIVITY = 0.003f;
	const float PLAYER_VELOCITY = 3.0f;
	const float GRAVITY = 10.0f;
	const float JUMP_VELOCITY = 4.0f;

	const float ENEMY_DETECTION_RADIUS = 10.0f;

	const int GAME_WIDTH =1280;
	const int GAME_HEIGHT = 960;
	const float FIXED_TIMESTEP = 1 / 60.0f; // 60 fps
	const int MAX_TIMESTEPS = 6;

	const float OFFSET = 0.0000001f;

	const int SPRITE_COUNT_X = 1;
	const int SPRITE_COUNT_Y = 1;
	const float TILE_SIZE = 1.0f;
	const int TILE_SIZE_PX = 512;

	// Blood System
	const int BLOOD_NUM_PARTICLES = 10;
	const float BLOOD_GRAVITY = 4.0f;

	// Fire System
	const int FIRE_NUM_PARTICLES = 20;
}