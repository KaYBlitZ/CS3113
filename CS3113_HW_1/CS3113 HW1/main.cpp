/*
	@author: Kenneth Liang

	CS3113 HW 1
*/

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

SDL_Window* displayWindow;

void initialize() {
	// set clear color as white
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// set view port
	glMatrixMode(GL_VIEWPORT);
	glViewport(0, 0, 800, 600);

	// use orthographic projection
	glMatrixMode(GL_PROJECTION);
	glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
}

GLuint LoadTexture(const char *image_path) {
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

void DrawSprite(GLint texture, float x, float y, float rotation) {
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, texture);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(x, y, 0.0);
	glRotatef(rotation, 0.0, 0.0, 1.0);

	GLfloat quad[] = { -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f };
	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);

	GLfloat quadUVs[] = { 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0 };
	glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_QUADS, 0, 4);

	glDisable(GL_TEXTURE_2D);
}

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("CS3113 HW1 - Kenneth Liang", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

	initialize();

	// animation initialize
	float lastFrameTicks = 0.0f;
	float bluePos = 0.5f;
	float greenAngle = 0.0f;
	float orangePos = -0.5f;

	const float VELOCITY = 0.3f;
	const float ROTATIONAL_VELOCITY = 45.0f;

	bool done = false;
	SDL_Event event;

	while (!done) {
		glClear(GL_COLOR_BUFFER_BIT);

		// animation
		float ticks = (float) SDL_GetTicks() / 1000.0f; // in secs
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		bluePos += VELOCITY * elapsed;
		orangePos -= VELOCITY * elapsed;
		greenAngle += ROTATIONAL_VELOCITY * elapsed;

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		// creating triangle
		GLfloat triangle[] = { 0.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f };
		glVertexPointer(2, GL_FLOAT, 0, triangle);
		glEnableClientState(GL_VERTEX_ARRAY);

		// add vertex colors, rgb
		GLfloat triangleColors[] = {
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f
		};
		glColorPointer(3, GL_FLOAT, 0, triangleColors);
		glEnableClientState(GL_COLOR_ARRAY);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		// need to disable, otherwise the colors will affect the sprites
		glDisableClientState(GL_COLOR_ARRAY);

		// textures
		GLuint blueShip = LoadTexture("playerShip2_blue.png");
		DrawSprite(blueShip, bluePos, 0.5f, 0);
		if (bluePos > 1.5f) bluePos = -1.5f;

		// the colorful spinning ship
		GLfloat colors[] = {
			0.0f, 0.0f, 1.0f,
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f
		};
		glColorPointer(3, GL_FLOAT, 0, colors);
		glEnableClientState(GL_COLOR_ARRAY);

		GLuint greenShip = LoadTexture("playerShip2_green.png");
		DrawSprite(greenShip, 0, 0, greenAngle);
		if (greenAngle > 360) greenAngle -= 360;

		glDisableClientState(GL_COLOR_ARRAY);

		GLuint orangeShip = LoadTexture("playerShip2_orange.png");
		DrawSprite(orangeShip, orangePos, -0.5f, 0);
		if (orangePos < -1.5f) orangePos = 1.5f;

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}
		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}