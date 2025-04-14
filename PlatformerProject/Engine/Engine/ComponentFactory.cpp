#include "ComponentFactory.h"
#include "ComponentList.h"

namespace MyEngine {
	std::map<std::string, std::function<std::shared_ptr<MyEngine::Component>()>> ComponentFactory::_componentMap = {
		{ "CAMERA",[]() { return std::make_shared<ComponentCamera>(); }},
		{ "RIGIDBODY",[]() { return std::make_shared<ComponentPhysicsBody>(); }},
		{ "SPRITE_RENDERER",[]() { return std::make_shared<ComponentRendererSprite>(); }},
		{ "SOUND_SYSTEM",[]() { return std::make_shared<ComponentSoundSystem>(); }},
		{ "PLAYER_ANIMATOR",[]() { return std::make_shared<ComponentAnimator>(); }}
	};

	void ComponentFactory::RegisterComponentOfType(std::string typeId, std::function<std::shared_ptr<MyEngine::Component>()> builder) {
		_componentMap[typeId] = builder;
	}

	std::shared_ptr<MyEngine::Component> ComponentFactory::GetComponentOfType(std::string typeId) {
		return _componentMap[typeId]();
	}
}