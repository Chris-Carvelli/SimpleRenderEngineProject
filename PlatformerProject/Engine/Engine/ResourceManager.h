#include "sre/SpriteAtlas.hpp"

namespace MyEngine {
	class ResourceManager {
		ResourceManager() = delete;

		static std::map<std::string, std::shared_ptr<sre::SpriteAtlas>> _atlases;

	public:
		static void RegisterAtlas(std::string atlasId, std::shared_ptr<sre::SpriteAtlas> atlas);
		static std::shared_ptr<sre::SpriteAtlas> GetAtlas(std::string atlasId);
	};
}