#include "OpenGLControl.h"
#include <random>
#include <time.h>

#include <Windows.h>
#include <string>
using namespace std;

std::mt19937 rng((unsigned int)time(0));

string ExePath() {
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	string::size_type pos = string(buffer).find_last_of("\\/");
	return string(buffer).substr(0, pos);
}

int main(int argc, char * argv[])
{
	OpenGLControl openGLControl;
	printf("%s", ExePath().c_str());
	if(openGLControl.init())
	{
		openGLControl.run();
	}
}