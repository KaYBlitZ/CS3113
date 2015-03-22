#pragma once

#include <SDL_opengl.h>

class SheetSprite {
public:
	SheetSprite();
	SheetSprite(unsigned int textureID, float u, float v, float width, float height, float scale);
	SheetSprite(const SheetSprite& rhs);

	SheetSprite& operator=(const SheetSprite& rhs);

	void draw(float x, float y, float width, float height);

	float u, v, texWidth, texHeight, scale;
private:
	unsigned int textureID;
};