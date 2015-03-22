#include "Entity.h"

Entity::Entity() : x(0.0f), y(0.0f), width(1.0f), height(1.0f), xVel(0.0f), yVel(0.0f), xAccel(0.0f), yAccel(0.0f), remove(false) {}
Entity::Entity(float x, float y, float width, float height, SheetSprite sprite) : x(x), y(y), width(width), height(height), xVel(0.0f), yVel(0.0f), xAccel(0.0f), yAccel(0.0f),
	sprite(sprite), remove(false) {}
Entity::Entity(const Entity& rhs) : x(rhs.x), y(rhs.y), width(rhs.width), height(rhs.height), xVel(rhs.xVel), yVel(rhs.yVel), xAccel(rhs.xAccel), yAccel(rhs.yAccel),
sprite(rhs.sprite), remove(rhs.remove) {}

Entity& Entity::operator=(const Entity& rhs) {
	if (&rhs != this) {
		x = rhs.x;
		y = rhs.y;
		width = rhs.width;
		height = rhs.height;
		xVel = rhs.xVel;
		yVel = rhs.yVel;
		xAccel = rhs.xAccel;
		yAccel = rhs.yAccel;
		remove = rhs.remove;
		sprite = rhs.sprite;
	}
	return *this;
}

void Entity::render() {
	sprite.draw(x, y, width, height);
}

float Entity::getScaledWidth() { return width * sprite.scale; }
float Entity::getScaledHeight() { return height * sprite.scale; }