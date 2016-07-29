#include "SoundSystem.h"

SoundSystem::SoundSystem()
{
	engine = createIrrKlangDevice();
	engine->setDefault3DSoundMinDistance(10.0f);
	preloadAllSounds();
}

void SoundSystem::preloadAllSounds()
{
	engine->addSoundSourceFromFile("data/sounds/cannonShoot.wav", irrklang::ESM_AUTO_DETECT, true);
	engine->addSoundSourceFromFile("data/sounds/enemyShipHit.wav", irrklang::ESM_AUTO_DETECT, true);
	engine->addSoundSourceFromFile("data/sounds/enemyShipShoot.wav", irrklang::ESM_AUTO_DETECT, true);
	engine->addSoundSourceFromFile("data/sounds/shieldHit.wav", irrklang::ESM_AUTO_DETECT, true);
}

void SoundSystem::setPlayerCamera(Camera* camera)
{
	this->camera = camera;
}

void SoundSystem::playSound(std::string soundSource, glm::vec3 soundPosition)
{
	engine->setListenerPosition(vec3df(camera->eye.x, camera->eye.y, camera->eye.z),
		vec3df(camera->direction.x, camera->direction.y, camera->direction.z));
	engine->play3D(soundSource.c_str(), vec3df(soundPosition.x, soundPosition.y, soundPosition.z));
}