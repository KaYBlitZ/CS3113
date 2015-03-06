#include "Game.h"

using namespace std;

const float Game::FIXED_TIMESTEP = 0.0166666666f; // 60 fps
const int Game::MAX_TIMESTEPS = 6;
const float Game::GRAVITY = 1.0f;
const float Game::OFFSET = 0.0001f;

Game::Game() : displayWindow(nullptr), keyStates(nullptr), done(false), numKeys(0) {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

	glMatrixMode(GL_VIEWPORT);
	glViewport(0, 0, 800, 600);

	glMatrixMode(GL_PROJECTION);
	glOrtho(-1.33, 1.33, -1.0, 1.0, -1.0, 1.0);

	glClearColor(0.5f, 0.3f, 1.0f, 1.0f);

	spriteSheet = LoadTexture("spritesheet.png");
	fontSheet = LoadTexture("font.png");

	SheetSprite sprite(spriteSheet, 441.0f / 692.0f, 25.0f / 692.0f, 17.0f / 692.0f, 21.0f / 692.0f, 6.0f);
	player = Player(0.0f, 0.0f, sprite);

	SheetSprite platformSprite(spriteSheet, 25.0f / 692.0f, 94.0f / 692.0f, 21.0f / 692.0f, 21.0f / 692.0f, 1.0f);
	entities.push_back(new Platform(0.0f, 0.5f, 1.0f, 0.15f, platformSprite));

	entities.push_back(new Platform(-0.6f, 0.0f, 0.4f, 0.15f, platformSprite));
	entities.push_back(new Platform(0.6f, 0.0f, 0.4f, 0.15f, platformSprite));

	entities.push_back(new Platform(-0.6f, -0.5f, 0.8f, 0.15f, platformSprite));
	entities.push_back(new Platform(0.6f, -0.5f, 0.8f, 0.15f, platformSprite));

	entities.push_back(new Platform(-1.13f, -0.75f, 0.2f, 0.15f, platformSprite));
	entities.push_back(new Platform(1.13f, -0.75f, 0.2f, 0.15f, platformSprite));
	entities.push_back(new Platform(0.0f, -0.9f, 2.6666666f, 0.15f, platformSprite));

	SheetSprite keySprite(spriteSheet, 327.0f / 692.0f, 31.0f / 692.0f, 14.0f / 692.0f, 10.0f / 692.0f, 5.0f);
	keys.push_back(Key(0.0f, 0.7f, keySprite));
	keys.push_back(Key(0.2f, 0.7f, keySprite));
	keys.push_back(Key(-0.2f, 0.7f, keySprite));
	keys.push_back(Key(0.4f, 0.7f, keySprite));
	keys.push_back(Key(-0.4f, 0.7f, keySprite));

	keys.push_back(Key(-0.6f, 0.2f, keySprite));
	keys.push_back(Key(0.6f, 0.2f, keySprite));

	keys.push_back(Key(-0.6f, -0.3f, keySprite));
	keys.push_back(Key(0.6f, -0.3f, keySprite));

	keys.push_back(Key(-1.13f, -0.55f, keySprite));
	keys.push_back(Key(1.13f, -0.55f, keySprite));
	keys.push_back(Key(-0.9f, -0.7f, keySprite));
	keys.push_back(Key(-0.4f, -0.7f, keySprite));
	keys.push_back(Key(0.9f, -0.7f, keySprite));
	keys.push_back(Key(0.4f, -0.7f, keySprite));
}

Game::~Game() {
	for (Entity*& entity : entities) {
		delete entity;
	}
	entities.clear();

	keys.clear();
	SDL_Quit();
}

void Game::update() {
	float ticks = (float)SDL_GetTicks() / 1000.0f; // in secs
	float elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;

	float fixedElapsed = elapsed + timeLeftOver;
	if (fixedElapsed > FIXED_TIMESTEP) {
		fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
	}

	while (fixedElapsed >= FIXED_TIMESTEP) {
		fixedElapsed -= FIXED_TIMESTEP;
		fixedUpdate();
	}
	timeLeftOver = fixedElapsed;

	handleEvents();
	keyStates = SDL_GetKeyboardState(nullptr);

	if (keyStates[SDL_SCANCODE_Z]) {
		player.xAccel = -Player::X_ACCEL;
	}
	else if (keyStates[SDL_SCANCODE_X]) {
		player.xAccel = Player::X_ACCEL;
	}
	else {
		player.xAccel = 0.0f;
	}
}

void Game::fixedUpdate() {
	player.collideBottom = false;
	player.collideLeft = false;
	player.collideRight = false;
	player.collideTop = false;

	player.xVel = lerp(player.xVel, 0.0f, FIXED_TIMESTEP * Player::PLAYER_FRIC_X);
	player.yVel = lerp(player.yVel, 0.0f, FIXED_TIMESTEP * Player::PLAYER_FRIC_Y);

	player.xVel += player.xAccel * FIXED_TIMESTEP;
	player.yVel -= GRAVITY * FIXED_TIMESTEP;

	player.y += player.yVel * FIXED_TIMESTEP;
	for (Entity*& entity : entities) {
		if (checkRectCollision(&player, entity)) {
			float penetration = fabs(fabs(player.y - entity->y) - player.getScaledHeight() / 2 - entity->getScaledHeight() / 2) + OFFSET;

			if (player.y < entity->y) {
				player.y -= penetration;
				player.collideTop = true;
			}
			else {
				player.y += penetration;
				player.collideBottom = true;
			}
			player.yVel = 0.0f;
		}
	}

	if (player.y < -1.0f) {
		player.y = 0.0f;
		player.x = 0.0f;
	}

	player.x += player.xVel * FIXED_TIMESTEP;
	for (Entity*& entity : entities) {
		if (checkRectCollision(&player, entity)) {
			float penetration = fabs(fabs(player.x - entity->x) - player.getScaledWidth() / 2 - entity->getScaledWidth() / 2) + OFFSET;

			if (player.x < entity->x) {
				player.x -= penetration;
				player.collideRight = true;
			}
			else {
				player.x += penetration;
				player.collideLeft = true;
			}
			player.xVel = 0.0f;
		}
	}

	for (Key& key : keys) {
		if (checkRectCollision(&player, &key)) {
			key.remove = true;
			numKeys++;
		}
	}

	removeEntities();
}

void Game::render() {
	glClear(GL_COLOR_BUFFER_BIT);

	for (Entity*& entity : entities) {
		entity->render();
	}
	for (Key& key : keys) {
		key.render();
	}

	drawText("Keys: " + to_string(numKeys), -1.333333f, 0.9f, 2.0f);

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
			if (code == SDL_SCANCODE_M) {
				if (player.collideBottom) player.yVel = 1.5f;
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

	vector<Key>::iterator itrKey = keys.begin();
	while (itrKey != keys.end()) {
		if (itrKey->remove) {
			itrKey = keys.erase(itrKey);
		}
		else {
			itrKey++;
		}
	}
}

float Game::calcDistance(Entity& entity1, Entity& entity2) {
	return sqrt(pow(entity2.x - entity1.x, 2.0f) + pow(entity2.y - entity1.y, 1.0f));
}

bool Game::checkRectCollision(Entity* entity1, Entity* entity2) {
	float entity1X = entity1->x - entity1->getScaledWidth() / 2;
	float entity2X = entity2->x - entity2->getScaledWidth() / 2;
	float entity1Y = entity1->y - entity1->getScaledHeight() / 2;
	float entity2Y = entity2->y - entity2->getScaledHeight() / 2;

	if (entity1X + entity1->getScaledWidth() < entity2X ||
		entity2X + entity2->getScaledWidth() < entity1X ||
		entity1Y + entity1->getScaledHeight() < entity2Y ||
		entity2Y + entity2->getScaledHeight() < entity1Y) {
		return false;
	}

	return true;
}

float Game::lerp(float v0, float v1, float t) {
	return (1.0f - t) * v0 + t * v1;
}

bool Game::isDone() { return done; }

GLuint Game::LoadTexture(const char *image_path) {
	SDL_Surface *surface = IMG_Load(image_path);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

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
	glLoadIdentity();
	glTranslatef(x, y, 0.0);

	float width = 1.0f / cols;
	float height = 1.0f / rows;
	vector<float> vertexData;
	vector<float> texCoordData;
	vector<float> colorData;

	for (size_t i = 0; i < text.length(); i++) {
		float u = (((int)text[i]) % 16) / (float)cols;
		float v = (((int)text[i]) / 16) / (float)rows;

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