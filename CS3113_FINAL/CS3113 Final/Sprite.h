#pragma once

#include <SDL_opengl.h>

class Sprite {
public:
	Sprite();
	Sprite(GLuint textureId, float width, float height);
	Sprite(const Sprite &rhs);
	Sprite& operator=(const Sprite &rhs);

	void draw();

	float alpha;
	float width, height;
	GLuint textureId;
};