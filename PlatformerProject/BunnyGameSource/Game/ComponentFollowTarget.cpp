#include "ComponentFollowTarget.h"

#include "Engine/MyEngine.h"

void ComponentFollowTarget::Init(rapidjson::Value& serializedData) {
	_targetId = serializedData["target"].GetString();
	_offsetX = serializedData["offsetX"].GetFloat();
	_offsetY = serializedData["offsetY"].GetFloat();
	_lockX = serializedData["lockX"].GetBool();
	_lockY = serializedData["lockY"].GetBool();

	_target = MyEngine::Engine::GetInstance()->GetGameObject(_targetId);
}

void ComponentFollowTarget::Update(float deltaTime) {
	auto target = _target.lock();
	if (!target)
		return;
	
	auto gameObject = GetGameObject().lock();
	if (!gameObject)
		return;

	auto targetPos = target->GetPosition();
	auto pos = gameObject->GetPosition();

	if (!_lockX)
		pos.x = targetPos.x + _offsetX;
	if (!_lockY)
		pos.y = targetPos.y + _offsetY;

	gameObject->SetPosition(pos);
}