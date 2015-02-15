/* CS3113
	HW 2 - Pong
	
	@author Kenneth Liang
*/

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <iostream>
#include <ctime>

const float PADDLE_VELOCITY = 2.0f;
const float BALL_X_VELOCITY = 0.4f;
const float BALL_POS = -0.05f;
const float Y_VELOCITY_INCREASE = 0.3f;

const Uint8* keys;

struct Player {
	float x, y, width, height;

	Player(float x, float y, float width, float height) : x(x), y(y), width(width), height(height) {}

	void update() {
		if (y + height > 1.0f) {
			y = 1.0f - height;
		}
		else if (y < -1.0f) {
			y = -1.0f;
		}
	}

	void render() {
		GLfloat rectangle[] = { 
			x + width, y + height, 
			x, y + height, 
			x, y, 
			x + width, y 
		};

		glVertexPointer(2, GL_FLOAT, 0, rectangle);
		glDrawArrays(GL_QUADS, 0, 4);
	}
};

// ball physics:
// constant x velocity
// y velocity increases/decreases depending on velocity of paddle
struct Ball {
	float x, y, length, xVelocity, yVelocity;

	Ball(float x, float y, float length) : x(x), y(y), length(length), 
		xVelocity(BALL_X_VELOCITY * (rand() % 2 == 0 ? -1 : 1)),
		yVelocity((1 + (rand() % 5)) / 10.0f * (rand() % 2 == 0 ? -1 : 1)) {}

	void update(float& elapsed) {
		x += xVelocity * elapsed;
		y += yVelocity * elapsed;

		// top collision
		if (y + length > 1.0f) {
			y = 1.0f - length;
			yVelocity *= -1;
		}
		// bottom collision
		else if (y < -1.0f) {
			y = -1.0f;
			yVelocity *= -1;
		}
	}

	void render() {
		GLfloat rectangle[] = {
			x + length, y + length,
			x, y + length,
			x, y,
			x + length, y
		};

		glVertexPointer(2, GL_FLOAT, 0, rectangle);
		glDrawArrays(GL_QUADS, 0, 4);
	}
	
	void reset() {
		x = BALL_POS;
		y = BALL_POS;

		xVelocity = BALL_X_VELOCITY * (rand() % 2 == 0 ? -1 : 1);
		yVelocity = (1 + (rand() % 5)) / 10.0f * (rand() % 2 == 0 ? -1 : 1);
	}
};

SDL_Window* displayWindow;

Player *playerOne, *playerTwo;
Ball* ball;

int playerOneScore = 0;
int playerTwoScore = 0;

bool checkPlayerOneCollision() {
	if (ball->y > playerOne->y + playerOne->height ||
		ball->y + ball->length < playerOne->y ||
		ball->x > playerOne->x + playerOne->width ||
		ball->x + ball->length < playerOne->x) {
		return false;
	}

	return true;
}

bool checkPlayerTwoCollision() {
	if (ball->y > playerTwo->y + playerTwo->height ||
		ball->y + ball->length < playerTwo->y ||
		ball->x > playerTwo->x + playerTwo->width ||
		ball->x + ball->length < playerTwo->x) {
		return false;
	}

	return true;
}

void initialize() {
	// set black background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	glEnableClientState(GL_VERTEX_ARRAY);

	// set view port
	glMatrixMode(GL_VIEWPORT);
	glViewport(0, 0, 800, 600);

	// use orthographic projection
	glMatrixMode(GL_PROJECTION);
	glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);
}

void renderCenterLine() {
	float x = -0.025f;
	float y = -1.0f;
	float width = 0.05f;
	float height = 0.2f;
	float space = 0.1f;

	while (y < 1.0f) {
		GLfloat rectangle[] = {
			x + width, y + height,
			x, y + height,
			x, y,
			x + width, y
		};
		glVertexPointer(2, GL_FLOAT, 0, rectangle);
		glDrawArrays(GL_QUADS, 0, 4);

		y += height + space;
	}
}

void update(float& elapsed) {
	playerOne->update();
	playerTwo->update();
	ball->update(elapsed);

	// check goals
	if (ball->x < -1.0f) {
		playerTwoScore += 1;
		ball->reset();
	}
	else if (ball->x + ball->length > 1.0f) {
		playerOneScore += 1;
		ball->reset();
	}

	// collisions
	if (checkPlayerOneCollision()) {
		ball->xVelocity *= -1;

		if (keys[SDL_SCANCODE_A]) {
			ball->yVelocity += Y_VELOCITY_INCREASE;
		}
		else if (keys[SDL_SCANCODE_Z]) {
			ball->yVelocity -= Y_VELOCITY_INCREASE;
		}
	}

	if (checkPlayerTwoCollision()) {
		ball->xVelocity *= -1;

		if (keys[SDL_SCANCODE_K]) {
			ball->yVelocity += Y_VELOCITY_INCREASE;
		}
		else if (keys[SDL_SCANCODE_M]) {
			ball->yVelocity -= Y_VELOCITY_INCREASE;
		}
	}
}

void render() {
	playerOne->render();
	playerTwo->render();
	ball->render();

	renderCenterLine();
}

void handleInput(float& elapsed) {
	keys = SDL_GetKeyboardState(nullptr);

	// player one
	if (keys[SDL_SCANCODE_A]) {
		playerOne->y += PADDLE_VELOCITY * elapsed;
	}
	else if (keys[SDL_SCANCODE_Z]) {
		playerOne->y -= PADDLE_VELOCITY * elapsed;
	}

	// player two
	if (keys[SDL_SCANCODE_K]) {
		playerTwo->y += PADDLE_VELOCITY * elapsed;
	}
	else if (keys[SDL_SCANCODE_M]) {
		playerTwo->y -= PADDLE_VELOCITY * elapsed;
	}
}

void cleanup() {
	delete playerOne;
	delete playerTwo;
	delete ball;
}

void printScore(int& playerOneScore, int& playerTwoScore) {
	std::cout << "Score: " << playerOneScore << " - " << playerTwoScore << std::endl;
}

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("CS3113 HW2 - Kenneth Liang", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

	std::cout << "An imitation of Pong\n" << "Player 1: A & Z to move\n" << "Player 2: K & M to move\n";

	initialize();

	// animation initialize
	float lastFrameTicks = 0.0f;

	bool done = false;
	SDL_Event event;

	playerOne = new Player(-0.95f, -0.1f, 0.05f, 0.4f);
	playerTwo = new Player(0.9f, -0.1f, 0.05f, 0.4f);
	ball = new Ball(BALL_POS, BALL_POS, 0.05f);

	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
			else if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.scancode == SDL_SCANCODE_R) {
					// reset score
					playerOneScore = 0;
					playerTwoScore = 0;
					printScore(playerOneScore, playerTwoScore);
				}
			}
		}

		glClear(GL_COLOR_BUFFER_BIT);

		// animation
		float ticks = (float)SDL_GetTicks() / 1000.0f; // in secs
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		handleInput(elapsed);
		update(elapsed);
		render();

		SDL_GL_SwapWindow(displayWindow);
	}

	cleanup();
	SDL_Quit();
	return 0;
}