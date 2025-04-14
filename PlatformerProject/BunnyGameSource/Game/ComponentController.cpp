#include "ComponentController.h"

#include "glm/glm.hpp"
#include "SDL.h"

#include "Engine/MyEngine.h"
#include "Engine/Components/ComponentPhysicsBody.h"
#include "Engine/Components/ComponentSoundSystem.h"
#include "ComponentPlatform.h"

#include "ComponentJetpack.h"
#include "ComponentSound.h"
#include "PlatformManager.h"


void ComponentController::Init(rapidjson::Value& serializedData) {
	auto playerObject = GetGameObject().lock();
	if (!playerObject)
		return;

	_body = playerObject->FindComponent<ComponentPhysicsBody>();
	_animator = playerObject->FindComponent<ComponentAnimator>();
}

void ComponentController::Update(float deltaTime) {
	auto body = _body.lock();
	if (!body)
		return;
	auto playerObject = GetGameObject().lock();
	if (!playerObject)
		return;
	_points = playerObject->FindComponent<ComponentPoints>();
	if (_isDead) {
		// Ensure the physics body stays in sensor mode while dead;
		if (body) {
			body->_fixture->SetSensor(true);
		}
		return;
	}
	//Update UI for game time
	_gameTime += deltaTime;
	_points.lock()->SetGameTime(_gameTime);

	auto linearVelocity = body->getLinearVelocity();
	linearVelocity.x = _mov.x * _movSpeed;
	body->setLinearVelocity(linearVelocity);

	HandleJump(body);
	ScreenEdgePan(body);
	glm::vec3 currentPos = playerObject->GetPosition();

	//Check whether player has fallen too far
	if ((_playerMaxHeight - currentPos[1]) > _maxFallDistance) {
		_isDead = true;

		// Make the player's body a sensor so it falls through platforms
		auto physicsBody = GetGameObject().lock()->FindComponent<ComponentPhysicsBody>().lock();
		if (physicsBody) {
			physicsBody->_fixture->SetSensor(true);
		}
		auto currentVel = body->getLinearVelocity();
		body->setLinearVelocity(glm::vec2(0, currentVel.y));

		// Update other systems of death state
		_animator.lock()->ToggleDeath();
		_points.lock()->ToggleDeath();
	}

	// Handle jetpack timer and effect
	if (_isJetpackActive) {
		_jetpackTimer -= deltaTime;
		auto velocity = body->getLinearVelocity();

		// If falling (negative y velocity) or timer expired, end jetpack
		if (_jetpackTimer <= 0 || velocity.y < 0) {
			_isJetpackActive = false;
			body->_fixture->SetSensor(false);
			glm::vec3 currentPos = playerObject->GetPosition();
			body->setPosition(currentPos);
		}
	}
}

void ComponentController::HandleJump(std::shared_ptr<ComponentPhysicsBody> body) {
	if (_jump) {
		body->addImpulse(glm::vec2(0, _jumpStrength));
		_jump = false;
	}

	if (_jetpack) {
		body->addImpulse(glm::vec2(0, _jetpackStrength));
		_jetpack = false;
		_isJetpackActive = true;
		_jetpackTimer = _jetpackDuration;
		body->_fixture->SetSensor(true);  // Make player pass through platforms
	}
}

void ComponentController::ScreenEdgePan(std::shared_ptr<ComponentPhysicsBody> body) {
	auto playerObject = body->GetGameObject().lock();
	glm::vec3 currentPos = playerObject->GetPosition();
	if (currentPos[0] < -235) {
		glm::vec3 newPos = glm::vec3(537, currentPos[1], currentPos[2]);
		playerObject->SetPosition(newPos);
		body->setPosition(newPos);
	}
	if (currentPos[0] > 540) {
		glm::vec3 newPos = glm::vec3(-232, currentPos[1], currentPos[2]);
		playerObject->SetPosition(newPos);
		body->setPosition(newPos);
	}
	if (currentPos[1] > _playerMaxHeight) {
		_playerMaxHeight = currentPos[1];
	}
}

void ComponentController::KeyEvent(SDL_Event& event) {
	// Quitting
	if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q) {
		SDL_Event quitEvent;
		quitEvent.type = SDL_QUIT;
		SDL_PushEvent(&quitEvent);
		MyEngine::Engine::GetInstance()->ProcessEvents(quitEvent);
		return;
	}

	// Is the player dead and pressing R?
	if (_isDead && event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r) {
		Reset();
		return;
	}

	//Otherwise allow movement
	switch (event.key.keysym.sym) {
		case SDLK_a: {
			if (event.type == SDL_KEYDOWN) {
				_mov.x = -1;
			}
			if (event.type == SDL_KEYUP && _mov.x < 0) {
				_mov.x = 0;
			}
			break;
		}
		case SDLK_d: {
			if (event.type == SDL_KEYDOWN) {
				_mov.x = 1;
			}
			if (event.type == SDL_KEYUP && _mov.x > 0) {
				_mov.x = 0;
			}
			break;
		}
	}
}

void ComponentController::OnCollisionStart(ComponentPhysicsBody* other, b2Manifold* manifold) {
	if (other == nullptr) {
		return;
	}
	//Get engine for handling platform destruction
	auto engine = MyEngine::Engine::GetInstance();

	//Get collision information
	auto collidedBody = other->GetGameObject().lock();
	auto collidedGameObject = collidedBody.get();
	auto soundCollision = collidedBody->FindComponent<ComponentSound>().lock();
	auto platformCollision = collidedBody->FindComponent<ComponentPlatform>().lock();
	auto jetpackCollision = collidedBody->FindComponent<ComponentJetpack>().lock();

	//Get player information and access to system components for playing SFX
	auto playerObject = GetGameObject().lock();
	if (soundCollision && !_isJetpackActive) {  // Only play sounds if not jetpacking
		if (!collidedBody) {
			return;
		}
		auto soundSystem = playerObject->FindComponent<ComponentSoundSystem>().lock();
		soundSystem->PlaySFX(soundCollision->GetCollisionSoundFile(), soundCollision->GetCollisionSoundVolume());
	}

	//Handle jumping and schedule object destruction.
	if (platformCollision) {
		if (!collidedBody) {
			return;
		}
		if (!_isJetpackActive) {  // Only jump if not in jetpack mode
			_jump = true;
			// Only destroy porous platforms if jetpack is not active
			if (platformCollision->porous) {
				engine->RegisterForDestruction(collidedGameObject);
				return;
			}
		}
	}
	else if (jetpackCollision) {
		if (!collidedBody) {
			return;
		}
		engine->RegisterForDestruction(collidedGameObject);
		_jetpack = true;
	}
}

void ComponentController::OnCollisionEnd(ComponentPhysicsBody* other, b2Manifold* manifold) {
	if (other == nullptr || manifold == nullptr) {
		return;
	}
	return;
}

void ComponentController::Reset() {
	auto body = _body.lock();
	if (!body) return;

	auto playerObject = GetGameObject().lock();
	if (!playerObject) return;

	// Clean up all platforms
	auto platformManager = GetGameObject().lock()->FindComponent<PlatformManager>().lock();
	if (platformManager) {
		platformManager->CleanupPlatforms();
	}

	// Reset position to starting point
	glm::vec3 startPos(100, 300, 0);
	playerObject->SetPosition(startPos);
	body->setPosition(startPos);
	body->setLinearVelocity(glm::vec2(0, 0));

	// Reset physics body's sensor state to enable collisions again
	auto physicsBody = playerObject->FindComponent<ComponentPhysicsBody>().lock();
	if (physicsBody) {
		physicsBody->_fixture->SetSensor(false);
	}

	// Reset systems back to ready state
	_animator.lock()->ToggleDeath();
	_points.lock()->ToggleDeath();

	// Reset game state variables
	_isDead = false;
	_playerMaxHeight = 0.0f;
	if (_gameTime > _bestTime) {
		_bestTime = _gameTime;
	}
	_gameTime = 0.0f;
	_mov = glm::vec3(0);
	_jump = false;
	_jetpack = false;
	_isJetpackActive = false;
	_jetpackTimer = 0.0f;
	_points.lock()->ResetPoints();
}