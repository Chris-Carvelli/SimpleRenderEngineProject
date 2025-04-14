#include "ComponentJetpack.h"
#include "ComponentPoints.h"
#include "Engine/MyEngine.h"
#include "Engine/Components/ComponentPhysicsBody.h"
#include "Engine/Components/ComponentRendererSprite.h"

void ComponentJetpack::Init(rapidjson::Value& serializedData) {
	_size = serializedData["size"].GetFloat();

	
	glm::vec2 sizeBody = glm::vec2(_size, _size);
	auto body = GetGameObject().lock()->CreateComponent<ComponentPhysicsBody>().lock();
	body->CreateBody(b2_kinematicBody, false, sizeBody);
}

void ComponentJetpack::Update(float deltaTime) {
	auto gameObject = GetGameObject().lock();
	if (!gameObject) return;

	auto engine = MyEngine::Engine::GetInstance();
	auto playerObject = engine->GetGameObject("Player");

	float platformHeight = gameObject->GetPosition()[1];
	auto points = playerObject.lock()->FindComponent<ComponentPoints>();
	float playerHeight = points.lock()->GetHeight();

	if (playerHeight > (platformHeight + 2000) && platformHeight > 700) {
		engine->RegisterForDestruction(this->GetGameObject().lock().get());
	}
}