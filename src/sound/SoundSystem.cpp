#include "SoundSystem.h"

SoundSystem::SoundSystem()
{
	engine = createIrrKlangDevice();
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