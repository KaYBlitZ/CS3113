#pragma once

#include <SDL_opengl.h>

class SheetSprite {
public:
	SheetSprite();
	SheetSprite(unsigned int textureID, float u, float v, float width, float height);
	SheetSprite(const SheetSprite& rhs);

	SheetSprite& operator=(const SheetSprite& rhs);

	void draw();

	float u, v, texWidth, texHeight;
private:
	unsigned int textureID;
};