#include "ComponentPlatformMove.h"

#include "Engine/MyEngine.h"
#include "Engine/Components/ComponentPhysicsBody.h"

void ComponentPlatformMove::Init(rapidjson::Value& serializedData) {
	auto gameObject = GetGameObject().lock();
	if (!gameObject)
		return;

	auto pos = gameObject->GetPosition();

	_yoyo = serializedData["yoyo"].GetBool();
	_duration = serializedData["duration"].GetFloat();
	_str = pos + MyEngine::GameObject::DeserializeVector(serializedData["start"]);
	_end = pos + MyEngine::GameObject::DeserializeVector(serializedData["end"]);
	_easing = static_cast<EasingType>(serializedData["easing"].GetInt());

	_body = gameObject->FindComponent<ComponentPhysicsBody>();
}

void ComponentPlatformMove::Update(float delta) {
	auto gameObject = GetGameObject().lock();
	if (!gameObject)
		return;

	_t = std::fmod(_t + delta, _duration);

	float t = _t / _duration;
	if (_yoyo) {
		t *= 2;
		if (t > 1)
			t = 2 - t;
	}

	float v = Easing(t);

	glm::vec3 pos = _str * (1 - v) + _end * v;


	glm::vec2 d = pos - gameObject->GetPosition();

	auto a = d * (1 / MyEngine::Engine::GetInstance()->PHYSICS_SCALE);
	_body.lock()->setLinearVelocity(d);

}

float ComponentPlatformMove::Easing(float t) {
	switch (_easing) {
		case Constant: return 0;
		case Linear: return t;
		case Sin: return glm::sin(t);

	}
}