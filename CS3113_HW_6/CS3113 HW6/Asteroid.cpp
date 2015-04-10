#include "Asteroid.h"

Asteroid::Asteroid() : Entity(0.0f, 0.0f, 0.0f), velocity(0.0f), shape(nullptr) {}

Asteroid::Asteroid(float x, float y, float rotation, float velocity, AsteroidSize size) : Entity(x, y, rotation), velocity(velocity), size(size) {
	switch (size) {
	case LARGE:
		radius = 0.2f;
		numPoints = 8;

		shape = new GLfloat[numPoints * 2] {
			0.0f, radius,
			-radius * cos(45 * Entity::degToRadRatio), radius * sin(45 * Entity::degToRadRatio),
			-radius, 0.0f,
			-radius * cos(45 * Entity::degToRadRatio), -radius * sin(45 * Entity::degToRadRatio),
			0.0f, -radius,
			radius * cos(45 * Entity::degToRadRatio), -radius * sin(45 * Entity::degToRadRatio),
			radius, 0.0f,
			radius * cos(45 * Entity::degToRadRatio), radius * sin(45 * Entity::degToRadRatio),
		};
		break;
	case MEDIUM:
		radius = 0.1f;
		numPoints = 7;

		shape = new GLfloat[numPoints * 2] {
			-radius, -radius / 3,
				-radius / 2, -radius / 2,
				0.0f, -radius,
				4 * radius / 5, -radius / 2,
				radius, 0.0f,
				radius / 2, radius,
				-radius / 2, radius
		};
		break;
	case SMALL:
		radius = 0.05f;
		numPoints = 5;

		shape = new GLfloat[numPoints * 2] {
			-4 * radius / 5, radius / 2,
				3 * radius / 5, 4 * radius / 5,
				radius, 0.0f,
				radius / 2, 4 * radius / 5,
				-4 * radius / 5, -radius / 2,
		};
		break;
	}
}

Asteroid::~Asteroid() {
	delete [] shape;
}

Asteroid::Asteroid(const Asteroid& rhs) : Entity(rhs), velocity(rhs.velocity), size(rhs.size), numPoints(rhs.numPoints) {
	switch (size) {
	case LARGE:
		shape = new GLfloat[numPoints * 2] {
			0.0f, radius,
				-radius * cos(45 * Entity::degToRadRatio), radius * sin(45 * Entity::degToRadRatio),
				-radius, 0.0f,
				-radius * cos(45 * Entity::degToRadRatio), -radius * sin(45 * Entity::degToRadRatio),
				0.0f, -radius,
				radius * cos(45 * Entity::degToRadRatio), -radius * sin(45 * Entity::degToRadRatio),
				radius, 0.0f,
				radius * cos(45 * Entity::degToRadRatio), radius * sin(45 * Entity::degToRadRatio),
		};
		break;
	case MEDIUM:
		shape = new GLfloat[numPoints * 2] {
			-radius, -radius / 3,
				-radius / 2, -radius / 2,
				0.0f, -radius,
				4 * radius / 5, -radius / 2,
				radius, 0.0f,
				radius / 2, radius,
				-radius / 2, radius
		};
		break;
	case SMALL:
		shape = new GLfloat[numPoints * 2] {
			-4 * radius / 5, radius / 2,
				3 * radius / 5, 4 * radius / 5,
				radius, 0.0f,
				radius / 2, 4 * radius / 5,
				-4 * radius / 5, -radius / 2,
		};
		break;
	}
}

Asteroid& Asteroid::operator=(const Asteroid& rhs) {
	if (this != &rhs) {
		Entity::operator=(rhs);
		velocity = rhs.velocity;
		size = rhs.size;
		numPoints = rhs.numPoints;
		switch (size) {
		case LARGE:
			shape = new GLfloat[numPoints * 2] {
				0.0f, radius,
					-radius * cos(45 * Entity::degToRadRatio), radius * sin(45 * Entity::degToRadRatio),
					-radius, 0.0f,
					-radius * cos(45 * Entity::degToRadRatio), -radius * sin(45 * Entity::degToRadRatio),
					0.0f, -radius,
					radius * cos(45 * Entity::degToRadRatio), -radius * sin(45 * Entity::degToRadRatio),
					radius, 0.0f,
					radius * cos(45 * Entity::degToRadRatio), radius * sin(45 * Entity::degToRadRatio),
			};
			break;
		case MEDIUM:
			shape = new GLfloat[numPoints * 2] {
				-radius, -radius / 3,
					-radius / 2, -radius / 2,
					0.0f, -radius,
					4 * radius / 5, -radius / 2,
					radius, 0.0f,
					radius / 2, radius,
					-radius / 2, radius
			};
			break;
		case SMALL:
			shape = new GLfloat[numPoints * 2] {
				-4 * radius / 5, radius / 2,
					3 * radius / 5, 4 * radius / 5,
					radius, 0.0f,
					radius / 2, 4 * radius / 5,
					-4 * radius / 5, -radius / 2,
			};
			break;
		}
	}
	return *this;
}

void Asteroid::fixedUpdate() {
	x += velocity * cos(rotation * Entity::degToRadRatio) * FIXED_TIMESTEP;
	y += velocity * sin(rotation * Entity::degToRadRatio) * FIXED_TIMESTEP;

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

void Asteroid::render() {
	createMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glMultMatrixf(matrix.ml);

	glVertexPointer(2, GL_FLOAT, 0, shape);
	glEnableClientState(GL_VERTEX_ARRAY);

	glLineWidth(3.0f);

	glDrawArrays(GL_POLYGON, 0, numPoints);

	glPopMatrix();
}