#include <iostream>
#include "ComponentSoundSystem.h"

//Had some memory leak issues so followed documentation to get something like this.
std::map<int, Mix_Chunk*> activeSounds;
void ChannelFinishedCallback(int channel) {
	if (activeSounds.find(channel) != activeSounds.end()) {
		Mix_FreeChunk(activeSounds[channel]); // Free the sound chunk (should alleviate our memory leak issue)
		activeSounds.erase(channel);          // Fix schedule basically
	}
}

void ComponentSoundSystem::Init(rapidjson::Value& serializedData) {
	int init = Mix_OpenAudio(
		22050,
		MIX_DEFAULT_FORMAT,
		2,
		2048
	);

	Mix_ChannelFinished(ChannelFinishedCallback);

	LoadMusic();
	PlayMusic();
}

void ComponentSoundSystem::Update(float deltaTime) {
	
}

void ComponentSoundSystem::LoadMusic() {
	music_file = Mix_LoadMUS("data/PackedDensityTime.mp3");
	if (!music_file) {
		std::cout << "Music Error: " << Mix_GetError();
	}
}

void ComponentSoundSystem::PlayMusic() {
	Mix_VolumeMusic(50);
	int playing = Mix_PlayMusic(music_file, -1); //Should loop it infinitely?
	if (!music_file) {
		std::cout << "Music Error: " << Mix_GetError();
	}
}

void ComponentSoundSystem::PlaySFX(std::string sound_file_name, int volume) {
	Mix_Chunk* sound_file = Mix_LoadWAV(sound_file_name.c_str());
	Mix_VolumeChunk(sound_file, volume);
	int channel = Mix_PlayChannel(
		-1, // int channel to play on (-1 is first available)
		sound_file,
		0 
	);

	activeSounds[channel] = sound_file;
}

ComponentSoundSystem::~ComponentSoundSystem() {
	for (auto& pair : activeSounds) {
		Mix_FreeChunk(pair.second);
	}
	activeSounds.clear();

	if (music_file) {
		Mix_FreeMusic(music_file);
		music_file = nullptr;
	}

	Mix_CloseAudio();
}