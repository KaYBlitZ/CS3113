#include "Game.h"

using namespace std;

const int LEVEL_WIDTH = 300;
const int LEVEL_HEIGHT = 28;
const float SCALE = 2.0f;
const int TILE_SIZE_PX = 21;
const float TILE_SIZE = TILE_SIZE_PX / 692.0f * SCALE;

const int Game::GAME_WIDTH = 800;
const int Game::GAME_HEIGHT = 600;
const float Game::FIXED_TIMESTEP = 0.0166666666f; // 60 fps
const int Game::MAX_TIMESTEPS = 6;
const float Game::GRAVITY = 0.2f;
const float Game::OFFSET = 0.0001f;
const int Game::SPRITE_COUNT_X = 30; // sprites in a row in spritesheet
const int Game::SPRITE_COUNT_Y = 30; // sprites in a col in spritesheet
const int Game::SPRITE_SPACING_PX = 2;
const int Game::SPRITE_MARGIN_PX = 2;

// origin starts at bottom left
Game::Game() : displayWindow(nullptr), keyStates(nullptr), done(false), numKeys(0) {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, GAME_WIDTH, GAME_HEIGHT, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

	//Mix_Init(MIX_INIT_MP3);
	//Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

	//jumpSound = Mix_LoadWAV("jump.wav");
	//shootSound = Mix_LoadWAV("shoot.wav");
	//music = Mix_LoadMUS("Dreamer.mp3");

	//Mix_PlayMusic(music, -1);

	glMatrixMode(GL_VIEWPORT);
	glViewport(0, 0, GAME_WIDTH, GAME_HEIGHT);

	glMatrixMode(GL_PROJECTION);
	glOrtho(-1.33, 1.33, -1.0, 1.0, -1.0, 1.0);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	spriteSheet = LoadTexture("spritesheet.png");
	fontSheet = LoadTexture("font.png");

	loadTiledData();
	loadTileMap();

	SheetSprite sprite(spriteSheet, 441.0f / 692.0f, 25.0f / 692.0f, 17.0f / 692.0f, 21.0f / 692.0f, SCALE);
	player = Player(TILE_SIZE * LEVEL_WIDTH / 2, -TILE_SIZE * LEVEL_HEIGHT / 2, sprite);
}

Game::~Game() {
	for (Entity*& entity : entities) {
		delete entity;
	}
	entities.clear();

	for (int i = 0; i < mapHeight; ++i) {
		delete levelData[i];
	}
	delete levelData;

	//Mix_FreeChunk(jumpSound);
	//Mix_FreeChunk(shootSound);
	//Mix_FreeMusic(music);

	//Mix_CloseAudio();
	//Mix_Quit();

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
	
	float bottomPoint = player.y - player.getHeight() / 2;
	int x, y;
	worldToTileCoordinates(player.x, bottomPoint, &x, &y);

	if (levelData[y][x] == 122 - 1) {
		float penetration = fabs(-y * TILE_SIZE - bottomPoint) + OFFSET;

		if (bottomPoint > -y * TILE_SIZE - TILE_SIZE / 2) {
			player.y += penetration;
			player.yVel = 0.0f;
			player.collideBottom = true;
		}
		else {
			player.y -= penetration;
			player.yVel = 0.0f;
			player.collideTop = true;
		}
	}

	if (player.y < TILE_SIZE * -LEVEL_HEIGHT) {
		player.y = TILE_SIZE * -LEVEL_HEIGHT / 2;
		player.x = TILE_SIZE * LEVEL_WIDTH / 2;
	}

	player.x += player.xVel * FIXED_TIMESTEP;
	
	float rightPoint = player.x + player.getWidth() / 2;
	worldToTileCoordinates(rightPoint, player.y, &x, &y);

	if (levelData[y][x] == 122 - 1) {
		float penetration = fabs(x * TILE_SIZE - rightPoint) + OFFSET;

		if (rightPoint < x * TILE_SIZE + TILE_SIZE / 2) {
			player.x -= penetration;
			player.xVel = 0.0f;
			player.collideLeft = true;
		}
		else {
			player.x += penetration;
			player.xVel = 0.0f;
			player.collideRight = true;
		}
	}

	removeEntities();
}

void Game::render() {
	glClear(GL_COLOR_BUFFER_BIT);

	renderTilemap();

	for (Entity*& entity : entities) {
		entity->render();
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-TILE_SIZE * LEVEL_WIDTH / 2, TILE_SIZE * LEVEL_HEIGHT / 2, 0.0f);
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
				if (player.collideBottom) {
					player.yVel = 1.5f;
					//Mix_PlayChannel(-1, jumpSound, 0);
				}
			}
			else if (code == SDL_SCANCODE_N) {
				//Mix_PlayChannel(-1, shootSound, 0);
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
}

float Game::calcDistance(Entity& entity1, Entity& entity2) {
	return sqrt(pow(entity2.x - entity1.x, 2.0f) + pow(entity2.y - entity1.y, 1.0f));
}

bool Game::checkRectCollision(Entity* entity1, Entity* entity2) {
	float entity1X = entity1->x - entity1->getWidth() / 2;
	float entity2X = entity2->x - entity2->getWidth() / 2;
	float entity1Y = entity1->y - entity1->getHeight() / 2;
	float entity2Y = entity2->y - entity2->getHeight() / 2;

	if (entity1X + entity1->getWidth() < entity2X ||
		entity2X + entity2->getWidth() < entity1X ||
		entity1Y + entity1->getHeight() < entity2Y ||
		entity2Y + entity2->getHeight() < entity1Y) {
		return false;
	}

	return true;
}

bool Game::checkTilemapCollision(float worldX, float worldY) {
	return false;
}

// remember, world coords in in openGL U and V coordinate system, not pixels!
void Game::worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY) {
	*gridX = (int)(worldX / TILE_SIZE);
	*gridY = (int)(-worldY / TILE_SIZE);
}

void Game::tileToWorldCoordinates(int gridX, int gridY, float *worldX, float *worldY) {
	*worldX = gridX * TILE_SIZE - TILE_SIZE * LEVEL_WIDTH / 2;
	*worldY = gridY * TILE_SIZE - TILE_SIZE * LEVEL_HEIGHT / 2;
}

float Game::lerp(float v0, float v1, float t) {
	return (1.0f - t) * v0 + t * v1;
}

void Game::loadTiledData() {
	ifstream map("level.txt");
	if (!map) {
		cerr << "Error loading Tiled file" << endl;
		return;
	}

	string line;
	while (getline(map, line)) {
		if (line == "[header]") {
			if (!readHeaderData(map)) {
				cerr << "Error loading header data" << endl;
				return;
			}
		}
		else if (line == "[layer]") {
			if (!readLayerData(map)) {
				cerr << "Error loading layer data" << endl;
			}
		}
	}
}

bool Game::readHeaderData(ifstream& stream) {
	string line;
	mapWidth = -1;
	mapHeight = -1;
	while (getline(stream, line)) {
		if (line == "") { break; }
		istringstream sStream(line);
		string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);
		if (key == "width") {
			mapWidth = atoi(value.c_str());
		}
		else if (key == "height"){
			mapHeight = atoi(value.c_str());
		}
	}
	if (mapWidth == -1 || mapHeight == -1) {
		return false;
	}
	else { // allocate our map data
		levelData = new int*[mapHeight];
		for (int i = 0; i < mapHeight; ++i) {
			levelData[i] = new int[mapWidth];
		}
		return true;
	}
}

bool Game::readLayerData(ifstream& stream) {
	string line;
	while (getline(stream, line)) {
		if (line == "") { break; }
		istringstream sStream(line);
		string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);
		if (key == "data") {
			for (int y = 0; y < mapHeight; y++) {
				getline(stream, line);
				istringstream lineStream(line);
				string tile;
				for (int x = 0; x < mapWidth; x++) {
					getline(lineStream, tile, ',');
					int val = (int) atoi(tile.c_str());
					if (val > 0) {
						// be careful, tiles start from index 1
						levelData[y][x] = val - 1;
					}
					else {
						levelData[y][x] = 0;
					}
				}
			}
		}
	}
	return true;
}

void Game::loadTileMap() {
	numTiles = 0;

	float spriteWidth = (1.0f - 2 * SPRITE_MARGIN_PX / 692.0f - (SPRITE_COUNT_X - 1) * SPRITE_SPACING_PX / 692.0f) / (float)SPRITE_COUNT_X;
	float spriteHeight = (1.0f - 2 * SPRITE_MARGIN_PX / 692.0f - (SPRITE_COUNT_Y - 1) * SPRITE_SPACING_PX / 692.0f) / (float)SPRITE_COUNT_Y;

	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {
			if (levelData[y][x] != 0) { // empty tile
				numTiles++;

				int i = levelData[y][x] % SPRITE_COUNT_X;
				float u = (i * TILE_SIZE_PX + SPRITE_MARGIN_PX + i * SPRITE_SPACING_PX) / 692.0f;
				int j = levelData[y][x] / SPRITE_COUNT_X;
				float v = (j * TILE_SIZE_PX + SPRITE_MARGIN_PX + j * SPRITE_SPACING_PX) / 692.0f;

				//float worldU = -1.33f + tileSize * x;
				//float worldV = 1.0f - tileSize * y;
				worldQuads.insert(worldQuads.end(), {
					TILE_SIZE * x, -TILE_SIZE * y,
					TILE_SIZE * x, (-TILE_SIZE * y) - TILE_SIZE,
					(TILE_SIZE * x) + TILE_SIZE, (-TILE_SIZE * y) - TILE_SIZE,
					(TILE_SIZE * x) + TILE_SIZE, -TILE_SIZE * y
				});
				/*worldQuads.insert(worldQuads.end(), {
					worldU, worldV,
					worldU, worldV - tileSize,
					worldU + tileSize, worldV - tileSize,
					worldU + tileSize, worldV,
				});*/
				texQuads.insert(texQuads.end(), {
					u, v,
					u, v + spriteHeight,
					u + spriteWidth, v + spriteHeight,
					u + spriteWidth, v
				});
			}
		}
	}
}

void Game::renderTilemap() {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, spriteSheet);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-TILE_SIZE * LEVEL_WIDTH / 2, TILE_SIZE * LEVEL_HEIGHT / 2, 0.0f);

	glVertexPointer(2, GL_FLOAT, 0, worldQuads.data());
	glEnableClientState(GL_VERTEX_ARRAY);

	glTexCoordPointer(2, GL_FLOAT, 0, texQuads.data());
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_QUADS, 0, 4 * numTiles);
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