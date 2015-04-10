#include "Player.h"

using namespace std;

const float Player::WIDTH = 0.05f;
const float Player::HEIGHT = 0.05f;
const float Player::RADIUS = 0.025f;
const float Player::ACCELERATION = 0.4f;
const float Player::DAMAGED_DURATION = 3.0f;

Player::Player() : isDamaged(false), damagedTime(0.0f), damagedState(0) {}

Player::Player(float x, float y, float rotation) : Entity(x, y, rotation, RADIUS), isDamaged(false), damagedTime(0.0f), damagedState(0) {
	shape = new GLfloat[6] { -WIDTH / 2.0f, HEIGHT / 2.0f,
		-WIDTH / 2.0f, -HEIGHT / 2.0f,
		WIDTH / 2.0f, 0.0f
	};
}

Player::~Player() {
	delete [] shape;
}

Player& Player::operator =(const Player& rhs) {
	if (&rhs != this) {
		Entity::operator=(rhs);
		shape = new GLfloat[6] { -WIDTH / 2.0f, HEIGHT / 2.0f,
			-WIDTH / 2.0f, -HEIGHT / 2.0f,
			WIDTH / 2.0f, 0.0f
		};
	}
	return *this;
}

void Player::fixedUpdate() {
	x += velocity.x * FIXED_TIMESTEP;
	y += velocity.y * FIXED_TIMESTEP;

	if (isDamaged) {
		damagedTime += FIXED_TIMESTEP;
		
		if (damagedTime > DAMAGED_DURATION) {
			isDamaged = false;
			damagedTime = 0.0f;
			damagedState = 0;
		}
	}

	if (x > 1.33f) {
		x = -1.33f;
	}
	else if (x < -1.33f) {
		x = 1.33f;
	}

	if (y > 1.0f) {
		y = -1.0f;
	}
	else if (y < -1.0f) {
		y = 1.0f;
	}
}

void Player::render() {
	glMatrixMode(GL_MODELVIEW);

	createMatrix();

	// MatrixMode is a stack, uses top of stack
	// since bottom of stack is always identity matrix and we dont go greater
	// a size of 2, no need to loadidentity
	glPushMatrix();
	glMultMatrixf(matrix.ml);

	glVertexPointer(2, GL_FLOAT, 0, shape);
	glEnableClientState(GL_VERTEX_ARRAY);

	if (isDamaged) {
		GLfloat color[12] = { 0.0f };

		if (damagedState == 0) {
			for (GLfloat& f : color) {
				f = 1.0f;
			}
			damagedState = 1;
		}
		else {
			for (size_t i = 0; i < 12; i++) {
				if (i % 4 == 2 || i % 4 == 3) { // meaning 3rd & 4th element
					color[i] = 1.0f;
				}
			}
			damagedState = 0;
		}

		glColorPointer(4, GL_FLOAT, 0, color);
		glEnableClientState(GL_COLOR_ARRAY);
	}

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableClientState(GL_COLOR_ARRAY);

	glPopMatrix(); // back to bottom or identity matrix
}