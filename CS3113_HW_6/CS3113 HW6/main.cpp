/*
	@author: Kenneth Liang
*/

#include "Game.h"

int main(int argc, char *argv[]) {
	Game game;
	
	while (!game.isDone()) {
		game.update();
		game.render();
	}
	
	return 0;
}