#pragma once

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <vector>
#include <string>
#include <math.h>
#include <limits.h>
#include <ctime>

#include "Entity.h"
#include "Player.h"
#include "Bullet.h"
#include "Asteroid.h"

extern const float FIXED_TIMESTEP;

class Game {
public:
	static const int MAX_TIMESTEPS;
	static const int GAME_HEIGHT, GAME_WIDTH;

	Game();
	virtual ~Game();

	void update();
	void fixedUpdate();
	void render();
	bool isDone();
private:
	void handleEvents();
	float calcDistance(Entity& entity1, Entity& entity2);
	bool checkCollision(Entity* entity1, Entity* entity2);
	void removeEntities();

	GLuint LoadTexture(const char *image_path);
	void drawText(std::string text, float x, float y, float scale);
	void drawText(int fontTexture, int rows, int cols, std::string text, float x, float y, float scale, float r, float g, float b, float a);

	SDL_Window* displayWindow;
	GLuint fontSheet;
	Mix_Chunk *jumpSound, *respawnSound;
	Mix_Music *music;

	Player player;
	std::vector<Entity*> entities;
	std::vector<Asteroid> asteroids;
	const Uint8* keyStates;
	float lastFrameTicks, timeLeftOver;
	bool done;
	int lives;
};