#include "ResourceManager.h"

namespace MyEngine {
	std::map<std::string, std::shared_ptr<sre::SpriteAtlas>> ResourceManager::_atlases = {};

	void ResourceManager::RegisterAtlas(std::string atlasId, std::shared_ptr<sre::SpriteAtlas> atlas) {
		_atlases[atlasId] = atlas;
	}

	std::shared_ptr<sre::SpriteAtlas> ResourceManager::GetAtlas(std::string atlasId) {
		auto ret = _atlases[atlasId];

		if (!ret)
		{
			ret = sre::SpriteAtlas::create("data/" + atlasId + ".json", "data/" + atlasId + ".png");
			RegisterAtlas(atlasId, ret);
		}

		return ret;
	}
}