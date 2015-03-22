#pragma once

#include "Entity.h"

class Platform : public Entity {
public:
	Platform();
	Platform(float x, float y, float width, float height, SheetSprite sprite);
};