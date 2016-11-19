#include "GameContext.h"
#include <random>
#include <time.h>

std::mt19937 rng((unsigned int)time(0));

int main(int argc, char * argv[])
{
	GameContext gameContext;
	if(gameContext.init())
	{
		gameContext.run();
	}
}