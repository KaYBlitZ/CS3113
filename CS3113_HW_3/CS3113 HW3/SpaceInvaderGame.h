#pragma once

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <vector>
#include <string>
#include "Player.h"
#include "Entity.h"
#include "Bullet.h"
#include "Enemy.h"

enum GameState {
	MAIN_MENU,
	PLAY_GAME,
	GAME_OVER_LOSE,
	GAME_OVER_WIN
};

class Enemy;

class SpaceInvaderGame {
public:
	SpaceInvaderGame();
	~SpaceInvaderGame();
	
	void update();
	void render();

	void shootEnemyBullet(float x, float y);

	bool isDone();
private:
	void init();
	GLuint LoadTexture(const char *image_path);
	void handleEvent();
	
	void shootMissile();
	void updateEnemies(float& elapsed);
	void checkCollsions();
	void removeEntities();

	void drawText(std::string text, float x, float y, float size);
	void drawText(int fontTexture, int rows, int cols,
		std::string text, float x, float y, float scale, float r, float g, float b, float a);
	
	void mainMenu();
	void gameOverLose();
	void gameOverWin();
	void game();
	void initGame();
	void reset();

	GameState gameState;
	int points, lives;
	GLuint spriteSheet, fontSheet;
	float lastFrameTicks, gameTime, moveDownTime;
	bool done, isEnemyMoveRight;
	SDL_Window* displayWindow;
	const Uint8* keys;
	// objects
	Player* player;
	std::vector<Enemy*> enemies;
	std::vector<Bullet*> bullets;
};