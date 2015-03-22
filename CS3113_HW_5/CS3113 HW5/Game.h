#pragma once

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include "Entity.h"
#include "Player.h"
#include "Platform.h"

class Entity;

class Game {
public:
	static const float FIXED_TIMESTEP;
	static const int MAX_TIMESTEPS;
	static const float GRAVITY;
	static const float OFFSET;
	static const int TILE_SIZE;
	static const int SPRITE_COUNT_X;
	static const int SPRITE_COUNT_Y;

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
	void loadTiledData();
	bool readHeaderData(std::ifstream& stream);
	bool readLayerData(std::ifstream& stream);
	void renderTilemap();
	void worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY);

	GLuint LoadTexture(const char *image_path);
	void drawText(std::string text, float x, float y, float scale);
	void drawText(int fontTexture, int rows, int cols, std::string text, float x, float y, float scale, float r, float g, float b, float a);

	SDL_Window* displayWindow;
	GLuint spriteSheet, fontSheet;
	Mix_Chunk *jumpSound, *shootSound;
	Mix_Music *music;

	Player player;
	std::vector<Entity*> entities;
	const Uint8* keyStates;
	float lastFrameTicks, timeLeftOver;
	int numKeys, mapWidth, mapHeight;
	int** levelData; // levelData[y][x]
	bool done;
};