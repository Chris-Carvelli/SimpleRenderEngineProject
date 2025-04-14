#include "ComponentSound.h"

void ComponentSound::Init(rapidjson::Value& serializedData) {
	_collisionSoundFile = serializedData["sound_file"].GetString();
	_soundVolume = serializedData["sound_volume"].GetInt();
}

std::string ComponentSound::GetCollisionSoundFile() {
	return _collisionSoundFile;
}

int ComponentSound::GetCollisionSoundVolume() {
	return _soundVolume;
}