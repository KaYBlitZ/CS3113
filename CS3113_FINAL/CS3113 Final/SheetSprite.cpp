#include "SheetSprite.h"

SheetSprite::SheetSprite() : textureID(0), u(0.0f), v(0.0f), texWidth(0.0f), texHeight(0.0f) {}

SheetSprite::SheetSprite(unsigned int textureID, float u, float v, float width, float height) :
textureID(textureID), u(u), v(v), texWidth(width), texHeight(height) {}

SheetSprite::SheetSprite(const SheetSprite& rhs) : textureID(rhs.textureID), u(rhs.u), v(rhs.v), texWidth(rhs.texWidth), texHeight(rhs.texHeight) {}

SheetSprite& SheetSprite::operator=(const SheetSprite& rhs) {
	if (&rhs != this) {
		textureID = rhs.textureID;
		u = rhs.u;
		v = rhs.v;
		texWidth = rhs.texWidth;
		texHeight = rhs.texHeight;
	}

	return *this;
}

void SheetSprite::draw() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);

	float w = texWidth / 2;
	float h = texHeight / 2;
	GLfloat objQuad[] = { -w, h, -w, -h, w, -h, w, h };
	GLfloat spriteQuad[] = { u, v, u, v + texHeight, u + texWidth, v + texHeight, u + texWidth, v };
	GLuint indices[] = { 0, 1, 2, 0, 2, 3 };

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, objQuad);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, spriteQuad);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
}