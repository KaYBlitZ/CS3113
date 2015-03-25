#include "SheetSprite.h"

SheetSprite::SheetSprite() : textureID(0), u(0.0f), v(0.0f), texWidth(0.0f), texHeight(0.0f), scale(0.0f) {}
SheetSprite::SheetSprite(unsigned int textureID, float u, float v, float width, float height, float scale) :
textureID(textureID), u(u), v(v), texWidth(width), texHeight(height), scale(scale) {}
SheetSprite::SheetSprite(const SheetSprite& rhs) : textureID(rhs.textureID), u(rhs.u), v(rhs.v), texWidth(rhs.texWidth), texHeight(rhs.texHeight), scale(rhs.scale) {}


SheetSprite& SheetSprite::operator=(const SheetSprite& rhs) {
	if (&rhs != this) {
		textureID = rhs.textureID;
		u = rhs.u;
		v = rhs.v;
		texWidth = rhs.texWidth;
		texHeight = rhs.texHeight;
		scale = rhs.scale;
	}
	return *this;
}

void SheetSprite::draw(float x, float y) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(x - TILE_SIZE * LEVEL_WIDTH / 2, y + TILE_SIZE * LEVEL_HEIGHT / 2, 0.0f);

	float w = texWidth / 2;
	float h = texHeight / 2;
	GLfloat worldQuad[] = { -w * scale, h * scale, -w * scale, -h * scale, w * scale, -h * scale, w * scale, h * scale };
	GLfloat spriteQuad[] = { u, v, u, v + texHeight, u + texWidth, v + texHeight, u + texWidth, v };

	glVertexPointer(2, GL_FLOAT, 0, worldQuad);
	glEnableClientState(GL_VERTEX_ARRAY);

	glTexCoordPointer(2, GL_FLOAT, 0, spriteQuad);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_QUADS, 0, 4);
}