#pragma once

#include "Engine/Component.h"
#include "SDL_mixer.h"

class ComponentSound : public MyEngine::Component {
	private:
		std::string _collisionSoundFile;
		int _soundVolume;
	public:
		virtual void Init(rapidjson::Value& serializedData) override;
		std::string GetCollisionSoundFile();
		int GetCollisionSoundVolume();
};