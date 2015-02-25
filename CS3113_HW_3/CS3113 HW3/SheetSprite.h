#pragma once

#include <SDL_opengl.h>

class SheetSprite {
public:
	SheetSprite();
	SheetSprite(unsigned int textureID, float u, float v, float width, float height, float scale);

	SheetSprite& operator=(const SheetSprite& rhs);

	void draw(float x, float y);

	float getWidth();
	float getHeight();
	float getScale();
private:
	float scale;
	unsigned int textureID;
	float u;
	float v;
	float width;
	float height;
};