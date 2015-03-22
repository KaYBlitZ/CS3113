#pragma once

#include "Entity.h"

class Key : public Entity {
public:
	Key();
	Key(float x, float y, SheetSprite sprite);
	Key(const Key& rhs);
};