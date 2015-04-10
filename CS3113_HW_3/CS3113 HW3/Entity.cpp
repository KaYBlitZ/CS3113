#include "Entity.h"

Entity::Entity(float x, float y) : x(x), y(y) {}
Entity::Entity(float x, float y, SheetSprite sprite) : x(x), y(y), sprite(sprite), remove(false) {}

Entity& Entity::operator=(const Entity& rhs) {
	if (&rhs != this) {
		x = rhs.x;
		y = rhs.y;
		sprite = rhs.sprite;
		remove = rhs.remove;
	}
	return *this;
}

bool Entity::canRemove() {
	return remove;
}

void Entity::render() {
	sprite.draw(x, y);
}

void Entity::setRemove(bool remove) {
	this->remove = remove;
}

float Entity::getX() {
	return x;
}

float Entity::getY() {
	return y;
}

float Entity::getWidth() {
	return sprite.getWidth();
}

float Entity::getHeight() {
	return sprite.getHeight();
}

float Entity::getScale() {
	return sprite.getScale();
}