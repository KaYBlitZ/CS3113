#include "Entity.h"

Entity::Entity(float x, float y, SheetSprite sprite) : x(x), y(y), sprite(sprite), remove(false) {}

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