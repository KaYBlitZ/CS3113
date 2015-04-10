/*
	@author: Kenneth Liang

	CS3113 HW 6
*/

#include "Game.h"

using namespace std;

const float FIXED_TIMESTEP = 1.0f / 60; // 60 fps

const int Game::GAME_WIDTH = 800;
const int Game::GAME_HEIGHT = 600;
const int Game::MAX_TIMESTEPS = 6;

// origin starts at bottom left
Game::Game() : displayWindow(nullptr), keyStates(nullptr), done(false), lives(3), jumpSound(nullptr), respawnSound(nullptr), music(nullptr) {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	displayWindow = SDL_CreateWindow("CS3113 HW6", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, GAME_WIDTH, GAME_HEIGHT, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

	srand(time(0));

	//Mix_Init(MIX_INIT_MP3);
	//Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

	//jumpSound = Mix_LoadWAV("jump.wav");
	//if (jumpSound == nullptr) {
	//	fprintf(stderr, "Unable to load jump WAV file: %s\n", Mix_GetError());
	//}

	//respawnSound = Mix_LoadWAV("respawn.wav");
	//if (respawnSound == nullptr) {
	//	fprintf(stderr, "Unable to load land WAV file: %s\n", Mix_GetError());
	//}

	//music = Mix_LoadMUS("music.mp3");
	//if (music == nullptr) {
	//	fprintf(stderr, "Unable to load MP3 file: %s\n", Mix_GetError());
	//}

	//Mix_PlayMusic(music, -1);

	glMatrixMode(GL_VIEWPORT);
	glViewport(0, 0, GAME_WIDTH, GAME_HEIGHT);

	glMatrixMode(GL_PROJECTION);
	glOrtho(-1.33, 1.33, -1.0, 1.0, -1.0, 1.0);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	fontSheet = LoadTexture("font.png");

	player = Player(0.0f, 0.0f, 90.0f);

	asteroids.push_back(Asteroid(-rand() % 10 / 10.0f, rand() % 10 / 10.0f, rand() % 360, rand() % 60 / 100.0f, LARGE));
	asteroids.push_back(Asteroid(rand() % 10 / 10.0f, rand() % 10 / 10.0f, rand() % 360, rand() % 60 / 100.0f, LARGE));
	asteroids.push_back(Asteroid(-rand() % 10 / 10.0f, -rand() % 10 / 10.0f, rand() % 360, rand() % 60 / 100.0f, LARGE));
}

Game::~Game() {
	for (Entity*& entity : entities) {
		delete entity;
	}
	entities.clear();
	asteroids.clear();

	//Mix_FreeChunk(jumpSound);
	//Mix_FreeChunk(respawnSound);
	//Mix_FreeMusic(music);

	//Mix_CloseAudio();
	//Mix_Quit();

	SDL_Quit();
}

void Game::update() {
	handleEvents();
	keyStates = SDL_GetKeyboardState(nullptr);

	float ticks = (float) SDL_GetTicks() / 1000.0f; // in secs
	float elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;

	float fixedElapsed = elapsed + timeLeftOver;
	if (fixedElapsed > MAX_TIMESTEPS *  FIXED_TIMESTEP) {
		fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
	}

	while (fixedElapsed >= FIXED_TIMESTEP) {
		fixedElapsed -= FIXED_TIMESTEP;
		fixedUpdate();
	}
	timeLeftOver = fixedElapsed;
}

void Game::fixedUpdate() {
	if (lives != 0 && keyStates[SDL_SCANCODE_Z]) {
		player.rotation += 180.0f * FIXED_TIMESTEP;
	}
	else if (lives != 0 && keyStates[SDL_SCANCODE_X]) {
		player.rotation -= 180.0f * FIXED_TIMESTEP;
	}

	if (lives != 0 && keyStates[SDL_SCANCODE_N]) {
		player.velocity.x += Player::ACCELERATION * cos(player.rotation * Entity::degToRadRatio) * FIXED_TIMESTEP;
		player.velocity.y += Player::ACCELERATION * sin(player.rotation * Entity::degToRadRatio) * FIXED_TIMESTEP;
	}

	for (Asteroid& asteroid : asteroids) {
		asteroid.fixedUpdate();
	}
	
	for (Entity*& entity : entities) {
		entity->fixedUpdate();
	}

	player.fixedUpdate();
	for (size_t i = 0; i < asteroids.size(); i++) {
		Asteroid* asteroid = &asteroids[i];

		if (lives != 0 && !player.isDamaged && checkCollision(asteroid, &player)) {
			lives -= 1;
			player.isDamaged = true;
		}
		for (Entity*& entity : entities) {
			if (checkCollision(entity, asteroid)) {
				asteroid->remove = true;
				entity->remove = true;
				if (asteroid->size == LARGE) {
					size_t num = rand() % 2 + 1;
					for (size_t i = 0; i < num; i++) {
						asteroids.push_back(Asteroid(asteroid->x, asteroid->y, rand() % 360, rand() % 80 / 100.0f, MEDIUM));
					}
				}
				else if (asteroid->size == MEDIUM) {
					size_t num = rand() % 2 + 1;
					for (size_t i = 0; i < num; i++) {
						asteroids.push_back(Asteroid(asteroid->x, asteroid->y, rand() % 360, rand() % 110 / 100.0f, SMALL));
					}
				}
			}
		}
	}

	removeEntities();
}

void Game::render() {
	glClear(GL_COLOR_BUFFER_BIT);

	drawText("Lives: " + to_string(lives), -1.33f, 0.85f, 2.0f);

	for (Asteroid& asteroid : asteroids) {
		asteroid.render();
	}

	for (Entity*& entity : entities) {
		entity->render();
	}

	player.render();

	SDL_GL_SwapWindow(displayWindow);
}

void Game::handleEvents() {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			done = true;
		}
		else if (event.type == SDL_KEYDOWN) {
			SDL_Scancode code = event.key.keysym.scancode;
			if (lives != 0 && code == SDL_SCANCODE_M) {
				entities.push_back(new Bullet(player.x, player.y, player.rotation, player.velocity));
				//Mix_PlayChannel(-1, jumpSound, 0);
			}
		}
	}
}

void Game::removeEntities() {
	vector<Entity*>::iterator itr = entities.begin();
	while (itr != entities.end()) {
		if ((*itr)->remove) {
			itr = entities.erase(itr);
		}
		else {
			itr++;
		}
	}

	vector<Asteroid>::iterator itr2 = asteroids.begin();
	while (itr2 != asteroids.end()) {
		if (itr2->remove) {			
			itr2 = asteroids.erase(itr2);
		}
		else {
			itr2++;
		}
	}
}

float Game::calcDistance(Entity& entity1, Entity& entity2) {
	return sqrt(pow(entity2.x - entity1.x, 2.0f) + pow(entity2.y - entity1.y, 1.0f));
}

bool Game::checkCollision(Entity* entity1, Entity* entity2) {
	// circle collision
	if (sqrtf(pow(entity1->x - entity2->x, 2) + pow(entity1->y - entity2->y, 2)) - entity1->radius - entity2->radius < 0) {
		return true;
	}

	return false;
}

bool Game::isDone() { return done; }

GLuint Game::LoadTexture(const char *image_path) {
	SDL_Surface *surface = IMG_Load(image_path);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	SDL_FreeSurface(surface);

	return textureID;
}

void Game::drawText(string text, float x, float y, float scale) {
	drawText(fontSheet, 16, 16, text, x, y, scale, 1.0f, 1.0f, 1.0f, 1.0f);
}

void Game::drawText(int fontTexture, int rows, int cols, string text, float x, float y, float scale, float r, float g, float b, float a) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, fontTexture);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(x, y, 0.0f);
	glScalef(scale, scale, 0.0f);

	float width = 1.0f / cols;
	float height = 1.0f / rows;
	vector<float> vertexData;
	vector<float> texCoordData;
	vector<float> colorData;

	for (size_t i = 0; i < text.length(); i++) {
		float u = (((int)text[i]) % 16) / (float)cols;
		float v = (((int)text[i]) / 16) / (float)rows;

		colorData.insert(colorData.end(), { r, g, b, a, r, g, b, a, r, g, b, a, r, g, b, a });
		float x = i * width;
		float w = width;
		float h = height;
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

	glDrawArrays(GL_QUADS, 0, text.length() * 4);

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	glPopMatrix();
}