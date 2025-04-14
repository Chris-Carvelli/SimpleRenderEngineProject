#pragma once

#include "Engine/Component.h"

class ComponentCamera : public MyEngine::Component {
public:
	void Init(rapidjson::Value& serializedData) override;
	void Update(float) override;
private:
	sre::Camera* _camera;
};
