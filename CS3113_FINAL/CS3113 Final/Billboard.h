#pragma once

#include "Entity.h"
#include "Sprite.h"

class Billboard : public Entity {
public:
	Billboard();
	Billboard(float x, float y, float z);
	Billboard(float x, float y, float z, const Sprite &sprite);
	Billboard(const Billboard &rhs);
	Billboard& operator=(const Billboard &rhs);

	void buildMatrix();
	void fixedUpdate();
	void update();
	void render();

	Sprite sprite;
};