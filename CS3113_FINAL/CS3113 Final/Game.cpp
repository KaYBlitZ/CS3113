#include "Game.h"

using namespace std;
using namespace Constants;

// origin starts at bottom left
Game::Game() : displayWindow(nullptr), keyStates(nullptr), done(false), state(GameState::MENU), emitters(50), ak47(nullptr), mp5(nullptr), music(nullptr) {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	displayWindow = SDL_CreateWindow("CS3113 Final - Kenneth Liang", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, GAME_WIDTH, GAME_HEIGHT, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

	Mix_Init(MIX_INIT_MP3);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

	srand(time(nullptr)); // call only once

	ak47 = Mix_LoadWAV("ak47.wav");
	if (ak47 == nullptr) {
		SDL_Log(("Unable to load jump WAV file: " + string(Mix_GetError())).c_str());
	}

	mp5 = Mix_LoadWAV("mp5.wav");
	if (mp5 == nullptr) {
		SDL_Log(("Unable to load land WAV file: " + string(Mix_GetError())).c_str());
	}

	music = Mix_LoadMUS("music.mp3");
	if (music == nullptr) {
		SDL_Log(("Unable to load MP3 file: " + string(Mix_GetError())).c_str());
	}

	Mix_PlayMusic(music, -1);

	glMatrixMode(GL_VIEWPORT);
	glViewport(0, 0, GAME_WIDTH, GAME_HEIGHT);

	glMatrixMode(GL_PROJECTION);
	glOrtho(-1.33f, 1.33f, -1.0f, 1.0f, -1.0f, 1.0f);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	grass = LoadTexture("grass.png");
	fontSheet = LoadTexture("font.png");
	tree1 = LoadTexture("tree1.png");
	tree2 = LoadTexture("tree2.png");
	tree3 = LoadTexture("tree3.png");
	blueFlower = LoadTexture("blue_flower.png");
	redFlower = LoadTexture("red_flower.png");
	enemy = LoadTexture("enemy.png");
	playerGun = LoadTexture("player_gun.png");
	fire = LoadTexture("fire.png");

	float width = 2.0f;
	float height = (373.0f / 588.0f) * width;
	player = Player(0.0f, 1.0f, 0.0f, Sprite(playerGun, width, height));
}

Game::~Game() {
	for (Entity *& entity : entities) {
		delete entity;
	}
	entities.clear();
	for (FireEmitter *& emitter : fireEmitters) {
		delete emitter;
	}
	fireEmitters.clear();
	enemies.clear();
	emitters.clear();
	vertices.clear();
	texcoords.clear();

	if (state == GameState::LEVEL_1 || state == GameState::LEVEL_2 || state == GameState::LEVEL_3) {
		for (int i = 0; i < mapHeight; ++i) {
			delete levelData[i];
		}
		delete levelData;
	}

	Mix_FreeChunk(ak47);
	Mix_FreeChunk(mp5);
	Mix_FreeMusic(music);

	Mix_CloseAudio();
	Mix_Quit();

	SDL_Quit();
}

void Game::update() {
	handleEvents();
	keyStates = SDL_GetKeyboardState(nullptr);

	float ticks = (float)SDL_GetTicks() / 1000.0f; // in secs
	float elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;

	float fixedElapsed = elapsed + timeLeftOver;
	if (fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
		fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
	}

	while (fixedElapsed >= FIXED_TIMESTEP) {
		fixedElapsed -= FIXED_TIMESTEP;
		fixedUpdate();
	}
	timeLeftOver = fixedElapsed;

	player.update();

	if (keyStates[SDL_SCANCODE_LCTRL]) {
		player.height = 0.5f;
	}
	else {
		player.height = 1.0f;
	}

	for (Entity*& entity : entities) {
		if (entity->isBillboard) {
			entity->rotation = Vector(
				player.position.x - entity->position.x,
				player.position.y - entity->position.y,
				player.position.z - entity->position.z,
				0.0f);
		}

		entity->update();
	}

	for (FireEmitter *&emitter : fireEmitters) {
		emitter->fixedUpdate();

		emitter->rotation = Vector(
			player.position.x - emitter->position.x,
			player.position.y - emitter->position.y,
			player.position.z - emitter->position.z,
			0.0f);

		emitter->update();
	}

	for (Enemy &enemy : enemies) {
		enemy.rotation = Vector(
			player.position.x - enemy.position.x,
			player.position.y - enemy.position.y,
			player.position.z - enemy.position.z,
			0.0f);

		enemy.update();

		float z = enemy.position.z - player.position.z;
		float x = enemy.position.x - player.position.x;
		float angle = atanf(x / z);

		Matrix rotateY, translate;

		rotateY.m[0][0] = cos(angle);
		rotateY.m[2][0] = -sin(angle);
		rotateY.m[0][2] = sin(angle);
		rotateY.m[2][2] = cos(angle);

		translate.ml[12] = enemy.position.x;
		translate.ml[13] = enemy.position.y;
		translate.ml[14] = enemy.position.z;

		Matrix transform = rotateY * translate;

		float halfWidth = enemy.sprite.width / 2;
		float halfHeight = enemy.sprite.height / 2;

		enemy.vertices[0] = transform * Vector(-halfWidth, halfHeight, 0.0f, 1.0f);
		enemy.vertices[1] = transform * Vector(-halfWidth, -halfHeight, 0.0f, 1.0f);
		enemy.vertices[2] = transform * Vector(halfWidth, -halfHeight, 0.0f, 1.0f);
		enemy.vertices[3] = transform * Vector(halfWidth, halfHeight, 0.0f, 1.0f);

		enemy.shootTime += elapsed;
		if (calcDistance(player, enemy) < ENEMY_DETECTION_RADIUS && enemy.shootTime > 0.3f + rand() % 5 / 10.0f) {
			enemy.shootTime = 0.0f;

			Mix_PlayChannel(-1, ak47, 0);
			
			if (rand() % 10 < 7) {
				player.hp -= rand() % 7 + 8;
			}
		}
	}

	if (player.hp < 0) {
		setState(GameState::GAME_OVER);
	}
}

void Game::fixedUpdate() {
	if (state == GameState::LEVEL_1 || state == GameState::LEVEL_2 || state == GameState::LEVEL_3) {
		if (player.canJump) { // player is on the ground; able to change velocity
			if (keyStates[SDL_SCANCODE_W]) {
				if (keyStates[SDL_SCANCODE_D]) {
					player.velocity.x = PLAYER_VELOCITY * sinf(player.rotation.y + 45 * DEG_TO_RAD);
					player.velocity.z = -PLAYER_VELOCITY * cosf(player.rotation.y + 45 * DEG_TO_RAD);
				}
				else if (keyStates[SDL_SCANCODE_A]) {
					player.velocity.x = PLAYER_VELOCITY * sinf(player.rotation.y - 45 * DEG_TO_RAD);
					player.velocity.z = -PLAYER_VELOCITY * cosf(player.rotation.y - 45 * DEG_TO_RAD);
				}
				else {
					player.velocity.x = PLAYER_VELOCITY * sinf(player.rotation.y);
					player.velocity.z = -PLAYER_VELOCITY * cosf(player.rotation.y);
				}
			}
			else if (keyStates[SDL_SCANCODE_S]) {
				if (keyStates[SDL_SCANCODE_D]) {
					player.velocity.x = -PLAYER_VELOCITY * sinf(player.rotation.y - 45 * DEG_TO_RAD);
					player.velocity.z = PLAYER_VELOCITY * cosf(player.rotation.y - 45 * DEG_TO_RAD);
				}
				else if (keyStates[SDL_SCANCODE_A]) {
					player.velocity.x = -PLAYER_VELOCITY * sinf(player.rotation.y + 45 * DEG_TO_RAD);
					player.velocity.z = PLAYER_VELOCITY * cosf(player.rotation.y + 45 * DEG_TO_RAD);
				}
				else {
					player.velocity.x = -PLAYER_VELOCITY * sinf(player.rotation.y);
					player.velocity.z = PLAYER_VELOCITY * cosf(player.rotation.y);
				}
			}
			else if (keyStates[SDL_SCANCODE_A]) {
				player.velocity.x = -PLAYER_VELOCITY * cosf(player.rotation.y);
				player.velocity.z = -PLAYER_VELOCITY * sinf(player.rotation.y);
			}
			else if (keyStates[SDL_SCANCODE_D]) {
				player.velocity.x = PLAYER_VELOCITY * cosf(player.rotation.y);
				player.velocity.z = PLAYER_VELOCITY * sinf(player.rotation.y);
			}
			else {
				player.velocity.x = 0.0f;
				player.velocity.z = 0.0f;
			}
		}

		// rotation buttons
		if (keyStates[SDL_SCANCODE_Q]) {
			player.rotation.y -= 70.0f * FIXED_TIMESTEP * DEG_TO_RAD;
		}
		if (keyStates[SDL_SCANCODE_E]) {
			player.rotation.y += 70.0f * FIXED_TIMESTEP * DEG_TO_RAD;
		}

		// update player y velocity
		player.fixedUpdate();

		int x, z;

		// check collision on x
		player.position.x += player.velocity.x * FIXED_TIMESTEP;
		worldToTileCoordinates(player.position.x, player.position.z, &x, &z);

		if (levelData[z][x] == -1) {
			float tileX = TILE_SIZE * x + TILE_SIZE / 2.0f;

			float offsetPositionX = player.position.x + TILE_SIZE * mapWidth / 2.0f;

			if (offsetPositionX < tileX) {
				player.position.x -= fabs(TILE_SIZE * x - offsetPositionX);
			}
			else if (offsetPositionX > tileX) {
				player.position.x += fabs(offsetPositionX - TILE_SIZE * (x + 1));
			}
		}

		// check collision on z
		player.position.z += player.velocity.z * FIXED_TIMESTEP;
		worldToTileCoordinates(player.position.x, player.position.z, &x, &z);

		if (levelData[z][x] == -1) {
			float tileZ = TILE_SIZE * z + TILE_SIZE / 2.0f;

			float offsetPositionZ = player.position.z + TILE_SIZE * mapHeight / 2.0f;

			if (offsetPositionZ < tileZ) {
				player.position.z -= fabs(TILE_SIZE * z - offsetPositionZ);
			}
			else if (offsetPositionZ > tileZ) {
				player.position.z += fabs(offsetPositionZ - TILE_SIZE * (z + 1));
			}
		}

		if (x == goalX && z == goalZ) {
			switch (state) {
			case GameState::LEVEL_1:
				setState(GameState::LEVEL_2);
				break;
			case GameState::LEVEL_2:
				setState(GameState::LEVEL_3);
				break;
			case GameState::LEVEL_3:
				setState(GameState::GAME_OVER);
				break;
			}
		}

		for (BloodEmitter &emitter : emitters) {
			emitter.fixedUpdate();
		}

		removeObjects();
	}
}

void Game::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	switch (state) {
	case GameState::MENU:
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		drawText("CS3113 - Final", -0.9f, 0.25f, 0.0f, 2.0f);
		drawText("Press Enter to Start", -1.3f, -0.5f, 0.0f, 2.0f);
		break;
	case GameState::LEVEL_1:
	case GameState::LEVEL_2:
	case GameState::LEVEL_3:
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glMultMatrixf(player.matrix.inverse().ml);

		renderTilemap();

		for (Entity *&entity : entities) {
			entity->render();
		}

		for (Enemy &enemy : enemies) {
			enemy.render();
		}

		for (BloodEmitter &emitter : emitters) {
			emitter.render();
		}

		for (FireEmitter *&emitter : fireEmitters) {
			emitter->render();
		}

		player.render();

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();

		glLoadIdentity();
		glOrtho(-1.33f, 1.33f, -1.0f, 1.0f, -1.0f, 1.0f);
		
		drawText("Health: " + to_string(player.hp), -1.33f, 0.90f, 0.0f, 1.5f);

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		break;
	case GameState::GAME_OVER:
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		drawText("GAME OVER", -0.55f, 0.25f, 0.0f, 2.0f);
		drawText("Press Enter", -0.7f, -0.5f, 0.0f, 2.0f);
		break;
	}

	SDL_GL_SwapWindow(displayWindow);
}

void Game::handleEvents() {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			done = true;
		}
		else if (event.type == SDL_MOUSEMOTION) {
			if (state == GameState::LEVEL_1 || state == GameState::LEVEL_2 || state == GameState::LEVEL_3) {
				player.rotation.x += -event.motion.yrel * MOUSE_SENSITIVITY;
				player.rotation.y += event.motion.xrel * MOUSE_SENSITIVITY;

				float halfPI = M_PI / 2.0f;

				if (player.rotation.x > halfPI) {
					player.rotation.x = halfPI;
				}
				else if (player.rotation.x < -halfPI) {
					player.rotation.x = -halfPI;
				}

				if (player.rotation.y > 2 * M_PI || player.rotation.y < -2 * M_PI) player.rotation.y = 0.0f;
			}
		}
		else if (event.type == SDL_MOUSEBUTTONDOWN) {
			if (state == GameState::LEVEL_1 || state == GameState::LEVEL_2 || state == GameState::LEVEL_3) {
				Uint8 button = event.button.button;

				if (button == SDL_BUTTON_LEFT) {
					shoot();
				}
			}
		}
		else if (event.type == SDL_KEYDOWN) {
			SDL_Scancode code = event.key.keysym.scancode;

			if (code == SDL_SCANCODE_ESCAPE) {
				done = true;
			}

			switch (state) {
			case GameState::MENU:
				if (code == SDL_SCANCODE_RETURN) {
					setState(GameState::LEVEL_1);

					setPerspective(60, 1.33f, 0.1f, 90.0f);
					if (SDL_SetRelativeMouseMode(SDL_bool::SDL_TRUE) == -1) SDL_Log(string("Relative Mouse Mode Error").c_str());
				}
				break;
			case GameState::LEVEL_1:
			case GameState::LEVEL_2:
			case GameState::LEVEL_3:
				if (code == SDL_SCANCODE_SPACE && player.canJump) {
					player.velocity.y = JUMP_VELOCITY;

					player.canJump = false;
				}
				break;
			case GameState::GAME_OVER:
				if (code == SDL_SCANCODE_RETURN) {
					setState(GameState::MENU);
					break;
				}
			}
		}
	}
}

void Game::shoot() {
	Mix_PlayChannel(-1, mp5, 0);

	for (Enemy &enemy : enemies) {
		float t;

		if (rayIntersectsTriangle(player.sight, enemy.vertices[0], enemy.vertices[1], enemy.vertices[2], t)) {
			emitters.push_back(BloodEmitter(player.sight.origin + player.sight.direction * t));
			enemy.hp -= rand() % 10 + 30;
		}
		else if (rayIntersectsTriangle(player.sight, enemy.vertices[0], enemy.vertices[2], enemy.vertices[3], t)) {
			emitters.push_back(BloodEmitter(player.sight.origin + player.sight.direction * t));
			enemy.hp -= rand() % 10 + 30;
		}

		if (enemy.hp < 0) {
			enemy.remove = true;
		}
	}
}

void Game::removeObjects() {
	vector<Entity*>::iterator itr = entities.begin();
	while (itr != entities.end()) {
		if ((*itr)->remove) {
			delete *itr;
			itr = entities.erase(itr);
		}
		else {
			itr++;
		}
	}

	vector<BloodEmitter>::iterator emitterItr = emitters.begin();
	while (emitterItr != emitters.end()) {
		if (emitterItr->remove) {
			emitterItr = emitters.erase(emitterItr);
		}
		else {
			emitterItr++;
		}
	}

	vector<Enemy>::iterator enemyItr = enemies.begin();
	while (enemyItr != enemies.end()) {
		if (enemyItr->remove) {
			enemyItr = enemies.erase(enemyItr);
		}
		else {
			enemyItr++;
		}
	}
}

void Game::setState(GameState state) {
	for (Entity *& entity : entities) {
		delete entity;
	}
	entities.clear();
	for (FireEmitter *& emitter : fireEmitters) {
		delete emitter;
	}
	fireEmitters.clear();
	enemies.clear();
	emitters.clear();
	vertices.clear();
	texcoords.clear();

	if (state == GameState::LEVEL_1 || state == GameState::LEVEL_2 || state == GameState::LEVEL_3) {
		for (int i = 0; i < mapHeight; ++i) {
			delete levelData[i];
		}
		delete levelData;
	}

	player.hp = 100;
	this->state = state;

	switch (state) {
	case GameState::LEVEL_1:
	case GameState::LEVEL_2:
	case GameState::LEVEL_3:
		loadTiledMap(this->state);
		break;
	case GameState::MENU:
	case GameState::GAME_OVER:
		SDL_SetRelativeMouseMode(SDL_bool::SDL_FALSE);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-1.33f, 1.33f, -1.0f, 1.0f, -1.0f, 1.0f);
		break;
	}
}

float Game::calcDistance(Entity &entity1, Entity &entity2) {
	return sqrt(pow(entity2.position.x - entity1.position.x, 2) + pow(entity2.position.y - entity1.position.y, 2) + pow(entity2.position.z - entity1.position.z, 2));
}

bool Game::rayIntersectsTriangle(Ray &ray, Vector &v1, Vector &v2, Vector &v3, float &t) {
	Vector e1, e2;
	Vector P, Q, T;
	float det, invDet, u, v;

	// edges v1 to v2 and v1 to v3
	e1 = v2 - v1;
	e2 = v3 - v1;

	// determine determinant, if near zero, ray lies in plane of triangle
	P = (ray.direction).cross(e2);
	det = e1.dot(P);

	if (det > -0.000001 && det < 0.000001) return false;
	invDet = 1.0f / det;

	// calculate distance from v1 to ray origin
	T = ray.origin - v1;

	u = T.dot(P) * invDet;

	// intersection lies outside of the triangle
	if (u < 0.0f || u > 1.0f) return false;

	// test v parameter
	Q = T.cross(e1);

	// cacluate v parameter
	v = (ray.direction).dot(Q) * invDet;

	if (v < 0.0f || u + v > 1.0f) return false;

	// at this stage we can compute t to find out where
	// the intersection point is on the line
	t = e2.dot(Q) * invDet;

	if (t > 0.000001f) { // ray intersection
		return true;
	}
	else {
		// this means that there is a line intersection
		// but not a ray intersection
		return false;
	}
}

void Game::setPerspective(float fovY, float aspect, float zNear, float zFar) {
	Matrix m;
	m.identity();
	m.m[0][0] = 1.0f / tanf(fovY * DEG_TO_RAD / 2.0f) / aspect;
	m.m[1][1] = 1.0f / tanf(fovY * DEG_TO_RAD / 2.0f);
	m.m[2][2] = (zFar + zNear) / (zNear - zFar);
	m.m[3][2] = (2.0f * zFar * zNear) / (zNear - zFar);
	m.m[2][3] = -1.0f;
	m.m[3][3] = 0.0f;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMultMatrixf(m.ml);
}

// remember, world coords are in openGL U and V coordinate system, not pixels!
void Game::worldToTileCoordinates(float worldX, float worldZ, int *gridX, int *gridZ) {
	// bug when returns wrong coords, resulting in collisions
	// might be a machine round error
	*gridX = (int)((worldX + TILE_SIZE * mapWidth / 2.0f) / TILE_SIZE);
	*gridZ = (int)((worldZ + TILE_SIZE * mapHeight / 2.0f) / TILE_SIZE);
}

void Game::loadTiledMap(GameState state) {
	ifstream map;

	switch (state) {
	case GameState::LEVEL_1:
		map = ifstream("level_1.txt");
		break;
	case GameState::LEVEL_2:
		map = ifstream("level_2.txt");
		break;
	case GameState::LEVEL_3:
		map = ifstream("level_3.txt");
		break;
	}

	if (!map) {
		SDL_Log(string("Error loading Tiled file").c_str());
		return;
	}

	string line;
	while (getline(map, line)) {
		if (line == "[header]") {
			if (!readHeaderData(map)) {
				SDL_Log(string("Error loading header data").c_str());
				return;
			}
		}
		else if (line == "[layer]") {
			if (!readLayerData(map)) {
				SDL_Log(string("Error loading layer data").c_str());
				return;
			}
		}
		else if (line == "[Objectlayer]") {
			if (!readObjectData(map)) {
				SDL_Log(string("Error loading object data").c_str());
				return;
			}
		}
	}

	numTiles = 0;

	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {
			if (levelData[y][x] != -1) { // not empty tile
				numTiles++;

				float u = (float)(((int)levelData[y][x]) % SPRITE_COUNT_X) / (float)SPRITE_COUNT_X;
				float v = (float)(((int)levelData[y][x]) / SPRITE_COUNT_X) / (float)SPRITE_COUNT_Y;

				float spriteWidth = 1.0f / SPRITE_COUNT_X;
				float spriteHeight = 1.0f / SPRITE_COUNT_Y;

				// origin (0, 0, 0) is top left of tile map
				vertices.insert(vertices.end(), {
					TILE_SIZE * x, 0.0f, TILE_SIZE * y,
					TILE_SIZE * x, 0.0f, (TILE_SIZE * y) + TILE_SIZE,
					(TILE_SIZE * x) + TILE_SIZE, 0.0f, (TILE_SIZE * y) + TILE_SIZE,
					(TILE_SIZE * x) + TILE_SIZE, 0.0f, TILE_SIZE * y
				});

				texcoords.insert(texcoords.end(), {
					u, v,
					u, v + spriteHeight,
					u + spriteWidth, v + spriteHeight,
					u + spriteWidth, v
				});
			}
		}
	}
}

bool Game::readObjectData(ifstream &stream) {
	string line, type;
	vector<int> x, y;

	while (getline(stream, line)) {
		if (line == "") { break; }

		istringstream sStream(line);
		string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);

		if (key == "type") {
			type = value;
		}
		else if (key == "x") {
			x.push_back(atoi(value.c_str()));
		}
		else if (key == "y") {
			y.push_back(atoi(value.c_str()));
		}
	}

	float deltaX = mapWidth / 2.0f * TILE_SIZE;
	float deltaZ = mapHeight / 2.0f * TILE_SIZE;

	if (type == "Flowers") {
		float height = 0.6f;

		for (size_t i = 0; i < x.size(); i++) {
			if (rand() % 2 == 0) {
				entities.push_back(new Billboard(
					x[i] / (float)TILE_SIZE_PX - deltaX,
					height / 2,
					y[i] / (float)TILE_SIZE_PX - deltaZ,
					Sprite(redFlower, height, height)
					));
			}
			else {
				entities.push_back(new Billboard(
					x[i] / (float)TILE_SIZE_PX - deltaX,
					height / 2,
					y[i] / (float)TILE_SIZE_PX - deltaZ,
					Sprite(blueFlower, height, height)
					));
			}
		}
	}
	else if (type == "Trees") {
		float height = 3.0f;

		for (size_t i = 0; i < x.size(); i++) {
			int val = rand() % 3;

			if (val == 0) {
				entities.push_back(new Billboard(
					x[i] / (float)TILE_SIZE_PX - deltaX,
					height / 2,
					y[i] / (float)TILE_SIZE_PX - deltaZ,
					Sprite(tree1, (410.0f / 439.0f) * height, height)
					));
			}
			else if (val == 1) {
				entities.push_back(new Billboard(
					x[i] / (float)TILE_SIZE_PX - deltaX,
					height / 2,
					y[i] / (float)TILE_SIZE_PX - deltaZ,
					Sprite(tree2, (362.0f / 502.0f) * height, height)
					));
			}
			else {
				entities.push_back(new Billboard(
					x[i] / (float)TILE_SIZE_PX - deltaX,
					height / 2,
					y[i] / (float)TILE_SIZE_PX - deltaZ,
					Sprite(tree3, (436.0f / 378.0f) * height, height)
					));
			}
		}
	}
	else if (type == "Enemies") {
		float height = 1.2f;

		for (size_t i = 0; i < x.size(); i++) {
			enemies.push_back(Enemy(
				x[i] / (float)TILE_SIZE_PX - deltaX,
				height / 2,
				y[i] / (float)TILE_SIZE_PX - deltaZ,
				Sprite(enemy, (177.0f / 395.0f) * height, height)
				));
		}
	}
	else if (type == "Spawn") {
		float _x = x[0] / (float)TILE_SIZE_PX - deltaX;
		float _y = y[0] / (float)TILE_SIZE_PX - deltaZ;
		player.position.x = _x;
		player.position.z = _y;

		fireEmitters.push_back(new FireEmitter(Vector(_x, 0.0f, _y, 0.0f), 0.5f, 0.5f, fire));
	}
	else if (type == "Goal") {
		float _x = x[0] / (float)TILE_SIZE_PX - deltaX;
		float _y = y[0] / (float)TILE_SIZE_PX - deltaZ;

		worldToTileCoordinates(_x, _y, &goalX, &goalZ);

		fireEmitters.push_back(new FireEmitter(Vector(_x, 0.0f, _y, 0.0f), 0.5f, 0.5f, fire));
	}
	return true;
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
						levelData[y][x] = -1; // empty
					}
				}
			}
		}
	}
	return true;
}

void Game::renderTilemap() {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, grass);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(-TILE_SIZE * mapWidth / 2.0f, 0.0f, -TILE_SIZE * mapHeight / 2.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0f);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices.data());
	
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, texcoords.data());

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_QUADS, 0, 4 * numTiles);

	glPopMatrix();

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

template<class A>
void Game::log(const A& a) {
	SDL_Log(to_string(a).c_str());
}

bool Game::isDone() { return done; }

GLuint Game::LoadTexture(const char *image_path) {
	SDL_Surface *surface = IMG_Load(image_path);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

	SDL_FreeSurface(surface);

	return textureID;
}

void Game::drawText(string text, float x, float y, float z, float scale) {
	drawText(fontSheet, 16, 16, text, x, y, z, scale, 0.0f, 0.0f, 0.0f, 1.0f);
}

void Game::drawText(int fontTexture, int rows, int cols, string text, float x, float y, float z, float scale, float r, float g, float b, float a) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, fontTexture);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	Matrix m;
	m.m[0][0] = scale;
	m.m[1][1] = scale;
	m.m[2][2] = scale;
	m.ml[12] = x;
	m.ml[13] = y;
	m.ml[14] = z;
	glMultMatrixf(m.ml);

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

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_FLOAT, 0, colorData.data());

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, vertexData.data());

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, texCoordData.data());

	glDrawArrays(GL_QUADS, 0, text.length() * 4);

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);

	glPopMatrix();
}