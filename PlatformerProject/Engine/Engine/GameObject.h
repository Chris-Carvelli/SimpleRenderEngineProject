#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <list>
#include <memory>
#include <string>

#include "sre/RenderPass.hpp"
#include "Box2D/Box2D.h"
#include "rapidjson/document.h"

class ComponentPhysicsBody;

namespace MyEngine {
	class Component;

	class GameObject {
		friend class Engine;

		// public API
	public:
		glm::mat4 transform;
		float rotation;

		void Init(rapidjson::Value& data);
		void Update(float);
		void Render(sre::RenderPass&);
		void KeyEvent(SDL_Event&);
		void OnCollisionStart(ComponentPhysicsBody* other, b2Manifold* manifold);
		void OnCollisionEnd(ComponentPhysicsBody* other, b2Manifold* manifold);

		void AddChild(std::shared_ptr<GameObject>);
		void AddComponent(std::shared_ptr<Component>);

		template<class T>
		std::weak_ptr<T> CreateComponent() {
			static_assert(std::is_base_of<Component, T>::value, "Template parameter type is not subclass of MyEngine::Component");
			
			auto component = std::make_shared<T>();
			AddComponent(component);

			return component;
		}

		std::string GetName();
		void SetName(std::string);

		template <class T>
		std::weak_ptr<T> FindComponent() {
			for(std::shared_ptr<Component> component : _components)
			{
				std::shared_ptr<T> candidate_ret = std::dynamic_pointer_cast<T>(component);
				if (candidate_ret != nullptr)
					return candidate_ret;
			}
			return std::weak_ptr<T>();
		}

	private:
		std::weak_ptr<GameObject> _parent;
		std::weak_ptr<GameObject> _self;
		std::list<std::weak_ptr<GameObject>> _children = {};
		std::list<std::shared_ptr<Component>> _components = {};
		std::string _name;

	public:
		glm::mat4 DeserializeTransform(rapidjson::Value& transformData);

		static glm::vec3 DeserializeVector(rapidjson::Value& vectorData);

		// transform
		glm::vec3 GetPosition();
		glm::quat GetRotation();
		glm::vec3 GetEulerAngles();
		glm::vec3 GetScale();
		void SetPosition(glm::vec3 position);
		void SetRotation(glm::quat rotation);
		void SetEulerAngles(glm::vec3 eulerAngles);
		void SetScale(glm::vec3 scale);
		void ResetTransform();
	};
}
