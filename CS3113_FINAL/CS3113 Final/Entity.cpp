#include "Entity.h"

Entity::Entity() : x(0.0f), y(0.0f), scaleX(0.0f), scaleY(0.0f), rotation(0.0f), remove(false) {}
Entity::Entity(float x, float y) : x(x), y(y), scaleX(1.0f), scaleY(1.0f), rotation(0.0f), remove(false) {}
Entity::Entity(const Entity& rhs) : matrix(rhs.matrix), x(rhs.x), y(rhs.y), scaleX(rhs.scaleX), scaleY(rhs.scaleY), rotation(rhs.rotation), remove(false) {}

Entity& Entity::operator=(const Entity& rhs) {
	if (&rhs != this) {
		// matrix recreated every render
		x = rhs.x;
		y = rhs.y;
		scaleX = rhs.scaleX;
		scaleY = rhs.scaleY;
		rotation = rhs.rotation;
		remove = rhs.remove;
	}
	return *this;
}

void Entity::createMatrix() {
	Matrix translate, rotate, scale;

	translate.ml[12] = x;
	translate.ml[13] = y;
	rotate.ml[0] = cos(rotation * Constants::DEG_TO_RAD);
	rotate.ml[4] = -sin(rotation * Constants::DEG_TO_RAD);
	rotate.ml[1] = sin(rotation * Constants::DEG_TO_RAD);
	rotate.ml[5] = cos(rotation * Constants::DEG_TO_RAD);
	scale.ml[0] = scaleX;
	scale.ml[5] = scaleY;

	matrix = scale * rotate * translate;
}