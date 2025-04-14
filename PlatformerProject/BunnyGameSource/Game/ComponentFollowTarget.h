#pragma once

#include "Engine/Component.h"

class ComponentFollowTarget : public MyEngine::Component {
	void Init(rapidjson::Value&) override;
	void Update(float deltaTime) override;

private:
	std::string _targetId;
	std::weak_ptr<MyEngine::GameObject> _target;
	float _offsetX;
	float _offsetY;
	bool _lockX;
	bool _lockY;
};