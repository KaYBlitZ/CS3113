/*
	CS3113 HW3
	@author: Kenneth Liang
*/

#include "SpaceInvaderGame.h"

int main(int argc, char *argv[]) {
	SpaceInvaderGame game;

	while (!game.isDone()) {
		game.update();
		game.render();
	}

	return 0;
}