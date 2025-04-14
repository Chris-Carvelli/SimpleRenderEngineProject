#include "sre/SpriteAtlas.hpp"

#include "Engine/MyEngine.h"
#include "Engine/ComponentFactory.h"

#include "Game/ComponentController.h"
#include "Game/ComponentFollowTarget.h"
#include "Game/ComponentPlatform.h"
#include "Game/ComponentPlatformMove.h"
#include "Game/ComponentJetpack.h"
#include "Game/PlatformManager.h"
#include "Game/ComponentSound.h"
#include "Game/ComponentPoints.h"

MyEngine::Engine engine;

int main() {
	MyEngine::ComponentFactory::RegisterComponentOfType("CONTROLLER", []() { return std::make_shared<ComponentController>(); });
	MyEngine::ComponentFactory::RegisterComponentOfType("FOLLOW_TARGET", []() { return std::make_shared<ComponentFollowTarget>(); });
	MyEngine::ComponentFactory::RegisterComponentOfType("PLATFORM_BUILDER", []() { return std::make_shared<ComponentPlatform>(); });
	MyEngine::ComponentFactory::RegisterComponentOfType("PLATFORM_MOVER", []() { return std::make_shared<ComponentPlatformMove>(); });
	MyEngine::ComponentFactory::RegisterComponentOfType("JETPACK", []() { return std::make_shared<ComponentJetpack>(); });
	MyEngine::ComponentFactory::RegisterComponentOfType("PLATFORM_MANAGER", []() { return std::make_shared<PlatformManager>(); });
	MyEngine::ComponentFactory::RegisterComponentOfType("COLLISION_SOUND", []() { return std::make_shared<ComponentSound>(); });
	MyEngine::ComponentFactory::RegisterComponentOfType("POINT_COUNTER", []() { return std::make_shared<ComponentPoints>(); });
	
	engine.Init("data/scene.json");
}
