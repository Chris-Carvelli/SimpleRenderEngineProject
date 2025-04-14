#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <chrono>

#include "sre/SDLRenderer.hpp"
#include "sre/Camera.hpp"
#include "sre/SpriteAtlas.hpp"

#include "SDL.h"

#include "Box2D/Box2D.h"
#include "Box2DDebugDraw.hpp"

#include "GameObject.h"

class ComponentPhysicsBody;

namespace MyEngine {

	class Engine : public b2ContactListener {
	private:
		static Engine* _instance;
	public:
		static Engine* GetInstance() { return _instance; }
	public:
		const glm::vec2 WIN_SIZE = glm::vec2(800,1000);
		const float PHYSICS_FIXED_FRAME_TIME = 1 / 60.0f;
		const int   PHYSICS_ITERATION_POSITION = 2;
		const int   PHYSICS_ITERATION_VELOCITY = 6;
		const float PHYSICS_SCALE = 100;
		std::vector<GameObject*> destructionQueue = {};
		std::vector<GameObject*> objectHolder = {};

		Engine();

		void Init(std::string sceneFile);
		void ProcessEvents(SDL_Event& event);
		void Update(float);
		void UpdatePhysics();
		void Render();
		void Reset();

		float GetFPS() const { return 1.0 / time_elapsed.count(); }
		float GetTimeElapsedMs() const { return 1000 * time_elapsed.count(); }
		float GetTimeComputationMs() const { return 1000 * time_computation.count(); }

		glm::vec2 GetScreenSize() const;

		int GetFrame() const { return frame; }
		float GetTime() const { return time; }
		sre::Camera* GetCamera() { return &_camera; };
		b2World* GetB2World() { return _b2World; }
		float GetPhysicsScale() { return PHYSICS_SCALE; }


		void RegisterPhysicsComponent(ComponentPhysicsBody* body);
		void DeregisterPhysicsComponent(ComponentPhysicsBody* body);
		void RegisterForDestruction(GameObject* gameObject);

		std::weak_ptr<GameObject> CreateGameObject(std::string name);
		std::weak_ptr<GameObject> CreateGameObject(std::string name, std::weak_ptr<GameObject> parent);
		std::weak_ptr<GameObject> GetGameObject(std::string name) { return _gameObjects[name]; }
		void DestroyGameObject(GameObject* gameObject);
		void DestroyQueuedBodies();
		void RegisterObject(GameObject* gameObject);

	private:
		// scene graph
		std::weak_ptr<GameObject> _root;
		std::map<std::string, std::shared_ptr<GameObject>> _gameObjects;

		// profiling and timing
		std::chrono::time_point<std::chrono::steady_clock>	time_start;
		std::chrono::time_point<std::chrono::steady_clock>	time_end;
		std::chrono::time_point<std::chrono::steady_clock>	time_end_computation;
		std::chrono::duration<double>						time_elapsed;
		std::chrono::duration<double>						time_computation;
		int frame;
		float time;

		// rendering
		sre::SDLRenderer _renderer;
		sre::Camera _camera;

		// physics
		b2World* _b2World;
		std::map<b2Fixture*, ComponentPhysicsBody*> _physicsLookup;
		void BeginContact(b2Contact* contact) override;
		void EndContact(b2Contact* contact) override;
		void HandleContact(b2Contact* contact, bool begin);

		// debug
		bool _showDebugWindow = false;
		bool _showDebugPhysics = false;
		Box2DDebugDraw _b2DebugDraw;

	};
}