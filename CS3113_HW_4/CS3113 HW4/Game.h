#pragma once

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <vector>
#include <string>
#include <math.h>
#include "Entity.h"
#include "Player.h"
#include "Platform.h"
#include "Key.h"

class Entity;

class Game {
public:
	static const float FIXED_TIMESTEP;
	static const int MAX_TIMESTEPS;
	static const float GRAVITY;
	static const float OFFSET;

	Game();
	virtual ~Game();

	void update();
	void fixedUpdate();
	void render();
	bool isDone();
private:
	void handleEvents();
	float calcDistance(Entity& entity1, Entity& entity2);
	bool checkRectCollision(Entity* entity1, Entity* entity2);
	void removeEntities();
	float lerp(float v0, float v1, float t);

	GLuint LoadTexture(const char *image_path);
	void drawText(std::string text, float x, float y, float scale);
	void Game::drawText(int fontTexture, int rows, int cols, std::string text, float x, float y, float scale, float r, float g, float b, float a);

	SDL_Window* displayWindow;
	GLuint spriteSheet, fontSheet;
	Player player;
	std::vector<Entity*> entities;
	std::vector<Key> keys;
	const Uint8* keyStates;
	float lastFrameTicks, timeLeftOver;
	int numKeys;
	bool done;
};