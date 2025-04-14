#pragma once

#include <map>
#include <string>

#include "Component.h"

namespace MyEngine {
	class ComponentFactory {
	private:
		ComponentFactory() = delete;

		static std::map<std::string, std::function<std::shared_ptr<MyEngine::Component>()>> _componentMap;
	public:
		static void RegisterComponentOfType(std::string typeId, std::function<std::shared_ptr<MyEngine::Component>()> builder);
		static std::shared_ptr<MyEngine::Component> GetComponentOfType(std::string typeId);
	};
}