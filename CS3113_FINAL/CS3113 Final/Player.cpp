#include "Player.h"

using namespace Constants;

Player::Player() : Entity(0.0f, 0.0f, 0.0f, false), canJump(false), hp(100), height(1.0f) {}

Player::Player(float x, float y, float z, const Sprite &gun) : Entity(x, y, z, false), canJump(false), gun(gun), hp(100), height(1.0f) {}

Player& Player::operator=(const Player& rhs) {
	if (&rhs != this) {
		Entity::operator=(rhs);
		velocity = rhs.velocity;
		canJump = rhs.canJump;
		gun = rhs.gun;
	}
	return *this;
}

void Player::fixedUpdate() {
	velocity.y -= GRAVITY * FIXED_TIMESTEP;
	position.y += velocity.y * FIXED_TIMESTEP;

	if (position.y < height) {
		position.y = height;
		velocity.y = 0.0f;
		canJump = true;
	}
}

void Player::update() {
	buildMatrix();

	sight.origin = Vector(position.x, position.y, position.z, 0.0f);
	sight.direction = matrix.inverse() * Vector(0.0f, 0.0f, -1.0f, 0.0f);
	sight.normalize();
}

void Player::render() {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glLoadIdentity();
	glOrtho(-1.33f, 1.33f, -1.0f, 1.0f, -1.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glLineWidth(2.0f);

	GLfloat crosshairVertices[] = { -0.1f, 0.0f, -0.05f, 0.0f, 0.0f, 0.05f, 0.0f, 0.1f, 0.05f, 0.0f, 0.1f, 0.0f, 0.0f, -0.05f, 0.0f, -0.1f };
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, crosshairVertices);

	GLfloat colors[] = { 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f };

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_FLOAT, 0, colors);

	glDrawArrays(GL_LINES, 0, 8);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glTranslatef(1.6f - gun.width / 2, -1.5f + gun.height / 2, 0.0f);

	gun.draw();

	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}