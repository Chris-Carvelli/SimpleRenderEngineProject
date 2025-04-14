#pragma once

#include "Engine/Component.h"


class ComponentPlatform : public MyEngine::Component {
private:
	glm::vec2 _platformSize = glm::vec2(380, 100);

public:
	virtual void Init(rapidjson::Value& serializedData) override;
	void Update(float deltaTime) override;
	bool porous;
};