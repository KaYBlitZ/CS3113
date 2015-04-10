#include "Entity.h"

const float Entity::degToRadRatio = M_PI / 180.0f;

Entity::Entity() : x(0.0f), y(0.0f), rotation(0.0f), radius(0.0f), scaleX(1.0f), scaleY(1.0f), remove(false) {}
Entity::Entity(float x, float y, float rotation) : Entity(x, y, rotation, 0.0f) {}
Entity::Entity(float x, float y, float rotation, float radius) : x(x), y(y), rotation(rotation), scaleX(1.0f), scaleY(1.0f), radius(radius), remove(false) {}
Entity::Entity(const Entity& rhs) : x(rhs.x), y(rhs.y), rotation(rhs.rotation), scaleX(rhs.scaleX), scaleY(rhs.scaleY), radius(rhs.radius), remove(rhs.remove) {}

Entity& Entity::operator=(const Entity& rhs) {
	if (&rhs != this) {
		x = rhs.x;
		y = rhs.y;
		rotation = rhs.rotation;
		scaleX = rhs.scaleX;
		scaleY = rhs.scaleY;
		radius = rhs.radius;
		remove = rhs.remove;
	}
	return *this;
}

void Entity::createMatrix() {
	Matrix translate, rotate, scale;

	translate.ml[12] = x;
	translate.ml[13] = y;
	rotate.ml[0] = cos(rotation * Entity::degToRadRatio);
	rotate.ml[4] = -sin(rotation * Entity::degToRadRatio);
	rotate.ml[1] = sin(rotation * Entity::degToRadRatio);
	rotate.ml[5] = cos(rotation * Entity::degToRadRatio);
	scale.ml[0] = scaleX;
	scale.ml[5] = scaleY;

	matrix = scale * rotate * translate;
}