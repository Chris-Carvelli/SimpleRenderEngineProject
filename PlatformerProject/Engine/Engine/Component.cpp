#include "Component.h"

#include "Logger.h"

namespace MyEngine {
	std::weak_ptr<GameObject> Component::GetGameObject() {
		return _gameObject;
	}

	void Component::SetGameObject(std::weak_ptr<GameObject> p_gameObject) {
		_gameObject = p_gameObject;
	}
}