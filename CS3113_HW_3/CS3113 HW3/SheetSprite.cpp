#include "SheetSprite.h"

SheetSprite::SheetSprite() : textureID(0), u(0), v(0), width(0), height(0) {}

SheetSprite::SheetSprite(unsigned int textureID, float u, float v, float width, float height, float scale) :
textureID(textureID), u(u), v(v), width(width), height(height), scale(scale) {}

SheetSprite& SheetSprite::operator=(const SheetSprite& rhs) {
	if (&rhs != this) {
		textureID = rhs.textureID;
		u = rhs.u;
		v = rhs.v;
		width = rhs.width;
		height = rhs.height;
		scale = rhs.scale;
	}
	return *this;
}

void SheetSprite::draw(float x, float y) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(x, y, 0.0);

	float w = width / 2;
	float h = height / 2;
	GLfloat quad[] = { -w * scale, h * scale, -w * scale, -h * scale,
		w * scale, -h * scale, w * scale, h * scale };
	GLfloat quadUVs[] = { u, v, u, v + height, u + width, v + height, u + width, v };

	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);

	glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_QUADS, 0, 4);
}

float SheetSprite::getWidth() {
	return width;
}

float SheetSprite::getHeight() {
	return height;
}

float SheetSprite::getScale() {
	return scale;
}