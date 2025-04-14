#include "MyEngine.h"

#include <random>
#include <fstream>

#include "sre/RenderPass.hpp"
#include <sre/Inspector.hpp>

#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "Engine/Components/ComponentPhysicsBody.h"
#include "Engine/Components/ComponentSoundSystem.h"

namespace MyEngine {
	Engine* Engine::_instance = nullptr;

	Engine::Engine() : _b2DebugDraw(PHYSICS_SCALE) {
		assert(_instance == nullptr && " Only one instance of MyEngine::Engine allowed!");
		_instance = this;
	}

	glm::vec2 Engine::GetScreenSize() const
	{
		return WIN_SIZE;
	}

	void Engine::Init(std::string sceneFile) {
		// initializes random generator
		std::srand(std::time(nullptr));

		// initialize sre
		_renderer.frameRender = [this]() { Render(); };
		_renderer.frameUpdate = [this](float deltaTime) {
			UpdatePhysics();
			Update(deltaTime);
		};
		_renderer.keyEvent = [this](SDL_Event& event) { ProcessEvents(event); };
		_renderer.setWindowSize(WIN_SIZE);
		_renderer.init();

		// initialize b2
		_b2World = new b2World(b2Vec2(0, -9.8));
		_b2World->SetContactListener(this);
		std::vector<GameObject*> destructionQueue = {};

		// load scene file
		std::ifstream fis(sceneFile);
		rapidjson::IStreamWrapper isw(fis);
		rapidjson::Document document;
		document.ParseStream(isw);
    
		_gameObjects["root"] = std::make_shared<GameObject>();
		_root = _gameObjects["root"];
		if (auto root = _root.lock()) {
			root->_parent = std::shared_ptr<GameObject>(nullptr); // we make sure that the root has no parent
			root->_self = root;
			root->Init(document["root"]);
		}
		_renderer.startEventLoop();
	}

	void Engine::ProcessEvents(SDL_Event& event) {
		if (event.key.keysym.sym == SDLK_F1 && event.type == SDL_KEYDOWN)
			_showDebugWindow = !_showDebugWindow;
		if (event.key.keysym.sym == SDLK_F2 && event.type == SDL_KEYDOWN) {
			_showDebugPhysics = !_showDebugPhysics;
			if (_showDebugPhysics) {
				_b2World->SetDebugDraw(&_b2DebugDraw);
			}
			else {
				_b2World->SetDebugDraw(nullptr);
			}
		}
		if (event.type == SDL_QUIT)
		{
			ImGuiIO io = ImGui::GetIO();
			io.BackendPlatformUserData = nullptr;
			io.BackendRendererUserData = nullptr;
			_renderer.stopEventLoop();
			_renderer.~SDLRenderer();
			_b2World->~b2World();
			exit(0);
		}

		if (auto root = _root.lock())
			root->KeyEvent(event);
	}

	void Engine::Update(float deltaTime) {
		++frame;
		time += deltaTime;

		if (auto root = _root.lock())
			root->Update(deltaTime);

		DestroyQueuedBodies();
	}

	void Engine::UpdatePhysics() {
		_b2World->Step(
			PHYSICS_FIXED_FRAME_TIME,
			PHYSICS_ITERATION_POSITION,
			PHYSICS_ITERATION_VELOCITY
		);

		for (auto kvp : _physicsLookup) {
			auto b2Body = kvp.first->GetBody();

			if (b2Body->GetType() == b2_staticBody)
				continue;

			if (kvp.second == nullptr)
				// invalid Component
				continue;

			auto physicsBody = kvp.second;

			const b2Vec2& position = b2Body->GetPosition();
			const float   angle = b2Body->GetAngle();
			if (auto gameObject = physicsBody->GetGameObject().lock())
			{
				gameObject->SetPosition(glm::vec3(position.x * PHYSICS_SCALE, position.y * PHYSICS_SCALE, 0));
				gameObject->SetEulerAngles(glm::vec3(0, 0, angle));
			}
		}
	}

	void Engine::Render()
	{
		sre::RenderPass renderPass = sre::RenderPass::create()
			.withCamera(_camera)
			.withClearColor(true, { .2f, .5f, 0.9, 1 })
			.build();

		if (auto root = _root.lock())
			root->Render(renderPass);

		static sre::Inspector profiler;

		if (_showDebugWindow)
		{
			profiler.update();
			profiler.gui(false);
		}

		if (_showDebugPhysics) {
			_b2World->DebugDraw();
			renderPass.drawLines(_b2DebugDraw.getLines());
			_b2DebugDraw.clear();
		}
	}

	void Engine::RegisterPhysicsComponent(ComponentPhysicsBody* body) {
		_physicsLookup[body->_fixture] = body;
	}

	void Engine::DeregisterPhysicsComponent(ComponentPhysicsBody* body) {
		if (!body) {
			return;
		}
		//std::string name = body->GetGameObject().lock()->GetName();
		_b2World->DestroyBody(body->_body);
		//std::cout << name << std::endl;
		auto iter = _physicsLookup.find(body->_fixture);
		if (iter != _physicsLookup.end())
			_physicsLookup.erase(iter);
	}

	std::weak_ptr<GameObject> Engine::CreateGameObject(std::string name) {
		//std::cout << "Created this GO: " << name << std::endl;
		assert(_gameObjects.find(name) == _gameObjects.end() && "Cannot create two objects with same name");

		auto ret = std::make_shared<GameObject>();
		_gameObjects[name] = ret;
		ret->_self = ret;
		ret->_parent = _root;
		ret->SetName(name);

		if(auto parent = _root.lock())
			parent->AddChild(ret);

		return _gameObjects[name];
	}

	std::weak_ptr<GameObject> Engine::CreateGameObject(std::string name, std::weak_ptr<GameObject> p_parent) {
		assert(_gameObjects.find(name) == _gameObjects.end() && "Cannot create two objects with same name");

		auto parent = p_parent.lock();
		if (!parent)
			return std::shared_ptr<GameObject>(nullptr);

		auto ret = std::make_shared<GameObject>();
		_gameObjects[name] = ret;
		ret->_self = ret;
		ret->_parent = p_parent;
		ret->SetName(name);
		ret->ResetTransform();
		parent->AddChild(ret);

		return _gameObjects[name];
	}

	void Engine::DestroyGameObject(GameObject* gameObject) {
		if (gameObject == nullptr)
			return;
		if (auto parent = gameObject->_parent.lock())
		{
			parent->_children.remove_if([gameObject](std::weak_ptr<GameObject> child) { return child.lock().get() == gameObject; });
		}
	}

	void Engine::BeginContact(b2Contact* contact) {
		b2ContactListener::BeginContact(contact);
		HandleContact(contact, true);
	}

	void Engine::EndContact(b2Contact* contact) {
		b2ContactListener::EndContact(contact);
		HandleContact(contact, false);
	}

	void Engine::HandleContact(b2Contact* contact, bool begin) {
		auto fixA = contact->GetFixtureA();
		auto fixB = contact->GetFixtureB();
		auto physA = _physicsLookup.find(fixA);
		auto physB = _physicsLookup.find(fixB);
		if (physA != _physicsLookup.end() && physB != _physicsLookup.end()) {
			auto gameObjA = physA->second->GetGameObject().lock();
			auto gameObjB = physB->second->GetGameObject().lock();

			// collision info
			b2Manifold*  manifold = contact->GetManifold();

			if (!gameObjA || !gameObjB)
				return;

			if (begin)
			{
				gameObjA->OnCollisionStart(physB->second, manifold);
				gameObjB->OnCollisionStart(physA->second, manifold);
			}
			else
			{
				gameObjA->OnCollisionEnd(physB->second, manifold);
				gameObjB->OnCollisionEnd(physA->second, manifold);
			}
		}
	}

	void Engine::RegisterObject(GameObject* gameObject) {
		objectHolder.push_back(gameObject);
	}

	void Engine::Reset() {
		for (auto* body : objectHolder) {
			RegisterForDestruction(body);
		}
	}

	void Engine::RegisterForDestruction(GameObject* gameObject) {
		destructionQueue.push_back(gameObject);
	}

	void Engine::DestroyQueuedBodies() {
		for (auto* body : destructionQueue) {
			if (body == nullptr) {
				continue;
			}
			objectHolder.erase(std::remove(objectHolder.begin(), objectHolder.end(), body),objectHolder.end());
			auto cpb = body->FindComponent<ComponentPhysicsBody>().lock().get();
			DeregisterPhysicsComponent(cpb);
			DestroyGameObject(body);
		}
		destructionQueue.clear();
	}
}