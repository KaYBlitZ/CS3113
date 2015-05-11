#pragma once

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <math.h>
#include <time.h>

#include "Constants.h"
#include "Matrix.h"
#include "Vector.h"
#include "Ray.h"
#include "Entity.h"
#include "Billboard.h"
#include "Enemy.h"
#include "Player.h"
#include "SheetSprite.h"
#include "BloodSystem.h"
#include "FireSystem.h"

class Entity;

enum GameState {
	MENU,
	LEVEL_1,
	LEVEL_2,
	LEVEL_3,
	GAME_OVER
};

class Game {
public:
	Game();
	virtual ~Game();

	void update();
	void fixedUpdate();
	void render();
	bool isDone();
private:
	void handleEvents();
	void setPerspective(float fovY, float aspect, float zNear, float zFar);

	float calcDistance(Entity& entity1, Entity& entity2);
	bool rayIntersectsTriangle(Ray &ray, Vector &v0, Vector &v1, Vector &v2, float &t);
	void shoot();
	void removeObjects();
	void setState(GameState state);

	void loadTiledMap(GameState state);
	bool readHeaderData(std::ifstream &stream);
	bool readLayerData(std::ifstream &stream);
	bool readObjectData(std::ifstream &stream);
	void renderTilemap();

	void worldToTileCoordinates(float worldX, float worldZ, int *gridX, int *gridZ);

	template<class A>
	void log(const A& a);
	GLuint LoadTexture(const char *image_path);
	void drawText(std::string text, float x, float y, float z, float scale);
	void drawText(int fontTexture, int rows, int cols, std::string text, float x, float y, float z, float scale, float r, float g, float b, float a);

	SDL_Window* displayWindow;
	GLuint grass, fontSheet, tree1, tree2, tree3, blueFlower, redFlower, enemy, playerGun, fire;
	GLuint level1Buffer, level2Buffer, level3Buffer;
	Mix_Chunk *ak47, *mp5;
	Mix_Music *music;

	GameState state;
	Matrix invPerspective;
	float x, y;
	Player player;
	std::vector<BloodEmitter> emitters;
	std::vector<FireEmitter*> fireEmitters;
	std::vector<Entity*> entities;
	std::vector<Enemy> enemies;
	std::vector<GLfloat> vertices, texcoords;
	const Uint8* keyStates;
	float lastFrameTicks, timeLeftOver;
	int mapWidth, mapHeight, numTiles, goalX, goalZ;
	int** levelData; // levelData[y][x]
	bool done;
};