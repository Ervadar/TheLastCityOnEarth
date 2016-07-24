#ifndef _SOUND_SYSTEM_H_
#define _SOUND_SYSTEM_H_

#include <string>
#include <irrKlang.h>
#include "Camera.h"

using namespace irrklang;

class SoundSystem
{
private:
	ISoundEngine* engine;
	Camera* camera;

public:
	SoundSystem();
	~SoundSystem() {};

	static SoundSystem& getInstance()
	{
		static SoundSystem soundSystemInstance;
		return soundSystemInstance;
	}
	SoundSystem(SoundSystem const&) = delete;
	void operator=(SoundSystem const&) = delete;

	void setPlayerCamera(Camera* camera);

	void playSound(std::string soundFileName, glm::vec3 soundPosition);
};

#endif