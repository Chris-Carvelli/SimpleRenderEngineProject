#pragma once

#include "Engine/Component.h"
#include "Engine/MyEngine.h"
#include "Engine/Components/ComponentAnimator.h"
#include "ComponentPoints.h"

class ComponentController : public MyEngine::Component {
	void Init(rapidjson::Value&) override;
	void Update(float deltaTime) override;
	void KeyEvent(SDL_Event&) override;
	void OnCollisionStart(ComponentPhysicsBody* other, b2Manifold* manifold) override;
	void OnCollisionEnd(ComponentPhysicsBody* other, b2Manifold* manifold) override;
	void ScreenEdgePan(std::shared_ptr<ComponentPhysicsBody> body);
	void HandleJump(std::shared_ptr<ComponentPhysicsBody> body);

private:
	std::weak_ptr<ComponentPhysicsBody> _body;
	std::weak_ptr<ComponentPoints> _points;
	std::weak_ptr<ComponentAnimator> _animator;
	glm::vec3 _mov;
	bool _jump;
	bool _jetpack;
	bool _isJetpackActive = false;
	float _jetpackTimer = 0.0f;
	bool _isDead = false;

	float _movSpeed = 3;
	float _jumpStrength = 5;
	float _jetpackStrength = 7;
	const float _jetpackDuration = 2.0f; // Duration of jetpack powerup
	float _maxFallDistance = 1100.0f; // Maximum distance player can fall before dying
	float _playerMaxHeight = 0.0f; // Tracks the player's highest point

	// Game time tracking
	float _gameTime = 0.0f;
	float _bestTime = 0.0f;

public:
	bool IsDead() const { return _isDead; }
	float GetGameTime() const { return _gameTime; }
	float GetBestTime() const { return _bestTime; }
	void Reset();
};