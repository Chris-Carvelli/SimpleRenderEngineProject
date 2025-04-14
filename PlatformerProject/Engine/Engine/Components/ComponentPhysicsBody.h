#pragma once

#include "Engine/Component.h"

#include <Box2D/Box2D.h>

class ComponentPhysicsBody : public MyEngine::Component {
	friend class MyEngine::Engine; 

public:
	~ComponentPhysicsBody();

	virtual void Init(rapidjson::Value& serializedData) override;

	void CreateBody(b2BodyType bodyType, bool isSensor, glm::vec2 size, float density=1);
	// Force gradually affects the velocity over time
	void addForce(glm::vec2 force);
	// Instantly affects velocity
	void addImpulse(glm::vec2 impulse);
	void setLinearVelocity(glm::vec2 velocity);
	void setPosition(glm::vec3);
	glm::vec2 getLinearVelocity();
	b2Fixture* _fixture = nullptr;

private:
	b2World* _world;
	b2Body* _body = nullptr;
	b2PolygonShape* _shape = nullptr;

	static glm::vec2 DeserializeVector2(rapidjson::Value& vectorData);
};