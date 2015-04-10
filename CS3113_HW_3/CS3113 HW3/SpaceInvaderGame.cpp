/*
	CS3113 HW3
	@author: Kenneth Liang
*/

#include "SpaceInvaderGame.h"

const SDL_Scancode KONAMI_CODE[] = { SDL_SCANCODE_UP, SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT,
SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, SDL_SCANCODE_B, SDL_SCANCODE_A };

using namespace std;

GLuint SpaceInvaderGame::LoadTexture(const char *image_path) {
	SDL_Surface *surface = IMG_Load(image_path);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SDL_FreeSurface(surface);

	return textureID;
}

void SpaceInvaderGame::drawText(string text, float x, float y, float scale) {
	drawText(fontSheet, 16, 16, text, x, y, scale, 1.0f, 1.0f, 1.0f, 1.0f);
}

void SpaceInvaderGame::drawText(int fontTexture, int rows, int cols, 
	string text, float x, float y, float scale, float r, float g, float b, float a) {

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, fontTexture);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(x, y, 0.0);

	float width = 1.0f / cols;
	float height = 1.0f / rows;
	vector<float> vertexData;
	vector<float> texCoordData;
	vector<float> colorData;

	for (size_t i = 0; i < text.length(); i++) {
		float u = (((int)text[i] ) % 16) / (float)cols;
		float v = (((int)text[i] ) / 16) / (float)rows;

		colorData.insert(colorData.end(), { r, g, b, a, r, g, b, a, r, g, b, a, r, g, b, a });
		float x = i * width * scale;
		float w = width * scale;
		float h = height * scale;
		vertexData.insert(vertexData.end(), {
			x, h,
			x, 0,
			x + w, 0,
			x + w, h
		});
		texCoordData.insert(texCoordData.end(), { u, v, u, v + height, u + width, v + height, u + width, v });
	}
	glColorPointer(4, GL_FLOAT, 0, colorData.data());
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, vertexData.data());
	glEnableClientState(GL_VERTEX_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, texCoordData.data());
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glDisableClientState(GL_COLOR_ARRAY);

	glDrawArrays(GL_QUADS, 0, text.length() * 4);
}

SpaceInvaderGame::SpaceInvaderGame() : gameState(MAIN_MENU), done(false), displayWindow(nullptr), isKonami(false) {
	init();
	// load texture once
	spriteSheet = LoadTexture("sheet.png");
	fontSheet = LoadTexture("font1.png");
}

SpaceInvaderGame::~SpaceInvaderGame() {
	SDL_Quit();
}

void SpaceInvaderGame::init() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("SpaceInvaderGame - CS3113 - Kenneth Liang", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // black
}

void SpaceInvaderGame::handleEvent() {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			done = true;
		}
		else if (event.type == SDL_KEYDOWN) {
			checkKonamiCode(event.key.keysym.scancode);
			if (event.key.keysym.scancode == SDL_SCANCODE_M) { // shoot missile
				if (gameState == PLAY_GAME) {
					if (player.canShoot() || isKonami) { // check if player can shoot - NO SPAMMING unless HAX
						shootMissile();
						player.setShoot(false);
					}
				}
			}
			else if (event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE) {
				gameState = MAIN_MENU;
			}
			else if (event.key.keysym.scancode == SDL_SCANCODE_RETURN) {
				switch (gameState) {
				case MAIN_MENU:
					resetAndInit();
					gameState = PLAY_GAME;
					// below line used to solve weird bug
					// without it, elapsed time on main menu will get added during start of play,
					// temporarily speeding up enemies depending on time spent on main menu
					lastFrameTicks = (float)SDL_GetTicks() / 1000.0f; // in secs
					break;
				case PLAY_GAME:
					break;
				case GAME_OVER_LOSE:
				case GAME_OVER_WIN:
					gameState = MAIN_MENU;
					break;
				}
			}
		}
	}
}

void SpaceInvaderGame::checkKonamiCode(SDL_Scancode& key) {
	if (key == KONAMI_CODE[konamiIndex]) {
		konamiIndex++;
	}
	else {
		konamiIndex = 0;
	}
	if (konamiIndex == 10) isKonami = true; // HAX on
}

void SpaceInvaderGame::update() {
	keys = SDL_GetKeyboardState(nullptr);
	handleEvent();

	float ticks = (float)SDL_GetTicks() / 1000.0f; // in secs
	float elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;

	if (gameState == PLAY_GAME) {

		gameTime += elapsed;
		if (gameTime > 40.0f) {
			ENEMY_MOVE_X = 1.0f;
		}
		else if (gameTime > 30.0f) {
			ENEMY_MOVE_X = 0.6f;
		}
		else if (gameTime > 20.0f) {
			ENEMY_MOVE_X = 0.2f;
		}

		if (enemies.size() == 0) gameState = GAME_OVER_WIN;

		player.update(elapsed, keys);
		for (Bullet& bullet : bullets) bullet.update(elapsed);
		updateEnemies(elapsed);

		checkCollsions();
		removeEntities();
	}
}

void SpaceInvaderGame::render() {
	glClear(GL_COLOR_BUFFER_BIT);

	switch (gameState) {
	case MAIN_MENU:
		mainMenu();
		break;
	case PLAY_GAME:
		game();
		break;
	case GAME_OVER_LOSE:
		gameOverLose();
		break;
	case GAME_OVER_WIN:
		gameOverWin();
		break;
	}
	
	SDL_GL_SwapWindow(displayWindow);
}

void SpaceInvaderGame::updateEnemies(float& elapsed) {
	bool moveDown = false;

	for (Enemy& enemy : enemies) {
		enemy.update(elapsed, isEnemyMoveRight, this);
		if (enemy.getX() > 0.9f || enemy.getX() < -0.9f) moveDown = true;
	}

	if (moveDown && gameTime - moveDownTime > 0.2f) {
		moveDowns++;
		moveDownTime = gameTime;

		isEnemyMoveRight = !isEnemyMoveRight;

		bool gameOver = false;
		for (Enemy& enemy : enemies) {
			enemy.move(0, -ENEMY_MOVE_Y);
			if (enemy.getY() < -0.8f) {
				gameOver = true;
			}
		}
		if (gameOver) {
			if (!isKonami) {
				gameState = GAME_OVER_LOSE;
			}
			else {
				for (Enemy& enemy : enemies) {
					enemy.move(0, moveDowns * ENEMY_MOVE_Y);
				}
				moveDowns = 0;
			}
		}
	}
}

void SpaceInvaderGame::shootMissile() {
	bullets.push_back(Bullet(player.getX(), player.getY(), SheetSprite(spriteSheet, 848 / 1024.f, 684 / 1024.f, 13 / 1024.f, 54 / 1024.f, 2.0f), PLAYER));
}

void SpaceInvaderGame::shootEnemyBullet(float x, float y) {
	bullets.push_back(Bullet(x, y, SheetSprite(spriteSheet, 740 / 1024.0f, 686 / 1024.0f, 37 / 1024.0f, 38 / 1024.0f, 1.0f), ENEMY));
}

void SpaceInvaderGame::checkCollsions() {
	for (Bullet& bullet : bullets) {
		switch (bullet.getType()) {
		case PLAYER:
			for (Enemy& enemy : enemies) {
				if (bullet.isColliding(&enemy)) {
					bullet.setRemove(true);
					enemy.setRemove(true);
					points += ENEMY_POINTS;
				}
			}
			break;
		case ENEMY:
			if (bullet.isColliding(&player)) {
				bullet.setRemove(true);
				if (!isKonami) lives -= 1;
				if (lives == 0) {
					gameState = GAME_OVER_LOSE;
				}
			}
			break;
		}
	}
}

void SpaceInvaderGame::removeEntities() {
	vector<Bullet>::iterator itr = bullets.begin();
	while (itr != bullets.end()) {
		if (itr->canRemove()) {
			itr = bullets.erase(itr);
		}
		else {
			itr++;
		}
	}
	vector<Enemy>::iterator itr2 = enemies.begin();
	while (itr2 != enemies.end()) {
		if (itr2->canRemove()) {
			itr2 = enemies.erase(itr2);
		}
		else {
			itr2++;
		}
	}
}

bool SpaceInvaderGame::isDone() {
	return done;
}

void SpaceInvaderGame::mainMenu() {
	drawText("SPACE INVADER", -1.0f, 0.5f, 2.0f);
	drawText("Kenneth Liang", -1.0f, 0.0f, 2.0f);
	drawText("PRESS ENTER TO PLAY", -1.0f, -0.5f, 1.5f);
	if (isKonami) drawText("SECRET ACTIVATED", -1.0f, -1.0f, 1.0f);
}

void SpaceInvaderGame::gameOverLose() {
	drawText("GAME OVER", -0.9f, 0.2f, 3.0f);
	drawText("MISSION UNSUCCESSFUL", -0.9f, 0.0f, 1.5f);
	drawText("PRESS ENTER", -0.5f, -0.2f, 1.5f);
}

void SpaceInvaderGame::gameOverWin() {
	drawText("GAME OVER", -0.9f, 0.2f, 3.0f);
	drawText("MISSION SUCCESSFUL", -0.9f, 0.0f, 1.5f);
	drawText("PRESS ENTER", -0.5f, -0.2f, 1.5f);
}

void SpaceInvaderGame::game() {
	for (Bullet& bullet : bullets) bullet.render();
	for (Enemy& enemy : enemies) enemy.render();
	player.render();
	drawText("Score: " + to_string(points), -1.0f, 0.85f, 1.5f);
	drawText("Lives: " + to_string(lives), -1.0f, -1.0f, 1.5f);
}

void SpaceInvaderGame::resetAndInit() {
	player = Player(0.0f, -0.85f, SheetSprite(spriteSheet, 224 / 1024.0f, 832 / 1024.0f, 99 / 1024.0f, 75 / 1024.0f, 2.0f));

	lives = isKonami ? 999 : 3;

	moveDowns = 0;
	ENEMY_MOVE_X = 0.1f;
	konamiIndex = 0;
	moveDownTime = 0.0f;
	gameTime = 0.0f;
	lastFrameTicks = 0.0f;
	points = 0;
	isEnemyMoveRight = true;

	bullets.clear();
	enemies.clear();

	//create enemies
	int cols = 7;
	int rows = 4;
	float w = 1.7f / cols;
	float h = 0.7f / rows;
	float startX = -0.8f;
	float startY = 0.2f;

	for (int i = 0; i < cols; i++) {
		for (int j = 0; j < rows; j++) {
			enemies.push_back(Enemy(w * i + startX, h * j + startY, SheetSprite(spriteSheet, 425 / 1024.0f, 552 / 1024.0f, 93 / 1024.0f, 84 / 1024.0f, 1.0f)));
		}
	}
}