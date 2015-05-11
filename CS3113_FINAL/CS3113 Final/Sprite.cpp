#include "Sprite.h"

Sprite::Sprite() : textureId(0), width(1.0f), height(1.0f), alpha(1.0f) {}

Sprite::Sprite(GLuint textureId, float width, float height) : textureId(textureId), width(width), height(height), alpha(1.0f) {}

Sprite::Sprite(const Sprite &rhs) : width(rhs.width), height(rhs.height), textureId(rhs.textureId), alpha(rhs.alpha) {}

Sprite& Sprite::operator=(const Sprite &rhs) {
	if (this != &rhs) {
		width = rhs.width;
		height = rhs.height;
		textureId = rhs.textureId;
		alpha = rhs.alpha;
	}
	
	return *this;
}

void Sprite::draw() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0f);
	//glDepthMask(GL_TRUE); //enabled by default

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureId);

	float halfWidth = width / 2;
	float halfHeight = height / 2;

	GLfloat vertices[] = { -halfWidth, halfHeight, -halfWidth, -halfHeight, halfWidth, -halfHeight, halfWidth, halfHeight };
	GLfloat colors[] = { 1.0f, 1.0f, 1.0f, alpha, 1.0f, 1.0f, 1.0f, alpha, 1.0f, 1.0f, 1.0f, alpha, 1.0f, 1.0f, 1.0f, alpha };
	GLfloat texcoords[] = { 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f };
	GLubyte indices[] = { 0, 1, 2, 0, 2, 3 };

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, vertices);

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_FLOAT, 0, colors);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, texcoords);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);
	//glDepthMask(GL_FALSE); // if false glClear(GL_DEPTH_BUFFER_BIT) won't have any effect
}