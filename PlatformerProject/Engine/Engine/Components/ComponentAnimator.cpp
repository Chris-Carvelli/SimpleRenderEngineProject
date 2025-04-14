#include "ComponentAnimator.h"

#include "Engine/MyEngine.h"

#include "Engine/ResourceManager.h"
#include <string>

void ComponentAnimator::Init(rapidjson::Value& serializedData) {
	_atlasName = serializedData["atlas"].GetString();

	_atlas = MyEngine::ResourceManager::GetAtlas(_atlasName);
	_spriteFalling = _atlas->get(serializedData["sprite_falling"].GetString());
	_spriteJumping = _atlas->get(serializedData["sprite_jumping"].GetString());
	_spriteDead = _atlas->get(serializedData["sprite_dead"].GetString());
	_spriteCurrent = _spriteFalling;
}

void ComponentAnimator::Update(float deltaTime) {
	auto gameObject = GetGameObject().lock();
	if (!gameObject)
		return;
	auto body = gameObject->FindComponent<ComponentPhysicsBody>().lock();
	glm::vec2 currentVelocity = body->getLinearVelocity();

	if (!isDead) {
		if (currentVelocity[1] > 0) {
			_spriteCurrent = _spriteJumping;
		}
		else {
			_spriteCurrent = _spriteFalling;
		}
	}
	else {
		_spriteCurrent = _spriteDead;
	}
}

void ComponentAnimator::Render(sre::RenderPass& renderPass) {
	auto gameObject = GetGameObject().lock();
	if (!gameObject)
		return;

	auto builder = sre::SpriteBatch::create();
	builder.addSprite(_spriteCurrent);
	auto batch = builder.build();
	renderPass.draw(batch, gameObject->transform);
}

void ComponentAnimator::SetSpritePosition(glm::vec2& pivot) {
	_spriteCurrent.setPosition(pivot);
}

void ComponentAnimator::ToggleDeath() {
	isDead = !isDead;
}