#pragma once

#include "Engine/Component.h"
#include "SDL_mixer.h"


class ComponentSoundSystem : public MyEngine::Component {

private:
	Mix_Music* music_file;

public:
	void Init(rapidjson::Value& serializedData);
	void Update(float deltaTime);
	void LoadMusic();
	void PlayMusic();
	void PlaySFX(std::string sound_file_name, int volume);
	
	~ComponentSoundSystem();
};

