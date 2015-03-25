#pragma once

#include <SDL_opengl.h>

extern const int LEVEL_WIDTH, LEVEL_HEIGHT;
extern const float TILE_SIZE;
extern const float HALF_LEVEL_WIDTH, HALF_LEVEL_HEIGHT;

class SheetSprite {
public:
	SheetSprite();
	SheetSprite(unsigned int textureID, float u, float v, float width, float height, float scale);
	SheetSprite(const SheetSprite& rhs);

	SheetSprite& operator=(const SheetSprite& rhs);

	void draw(float x, float y);

	float u, v, texWidth, texHeight, scale;
private:
	unsigned int textureID;
};