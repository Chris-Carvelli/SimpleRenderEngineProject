#include "GameObject.h"

#include "sre/RenderPass.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>

#include "MyEngine.h"
#include "Component.h"
#include "ComponentFactory.h"
#include "Logger.h"
#include "QuaternionsSafe.h"

namespace MyEngine {
	// public API
	void GameObject::Init(rapidjson::Value& data) {
		auto engine = Engine::GetInstance();

		_name = data["name"].GetString();
		transform = DeserializeTransform(data["transform"]);

		if (data.HasMember("components"))
			for (auto& componentData : data["components"].GetArray())
			{
				auto TMP_id = componentData["typeId"].GetString();
				auto component = ComponentFactory::GetComponentOfType(componentData["typeId"].GetString());
				AddComponent(component);
				component->Init(componentData["serializedData"]);
			}
		
		if (data.HasMember("children"))
			for (auto& childData : data["children"].GetArray())
			{
				auto child = engine->CreateGameObject(childData["name"].GetString(), _self);
				child.lock()->Init(childData);
			}
	}

	void GameObject::Update(float deltaTime) {
		for (auto& component : _components)
			component->Update(deltaTime);

		for (auto& child : _children)
			if(auto go = child.lock())
				go->Update(deltaTime);
	}

	void GameObject::Render(sre::RenderPass& renderPass) {
		for (auto& component : _components)
			component->Render(renderPass);

		for (auto& child : _children)
			if (auto go = child.lock())
				go->Render(renderPass);
	}

	void GameObject::KeyEvent(SDL_Event& e) {
		for (auto& component : _components)
			component->KeyEvent(e);

		for (auto& child : _children)
			if (auto go = child.lock())
				go->KeyEvent(e);
	}

	void GameObject::OnCollisionStart(ComponentPhysicsBody* other, b2Manifold* manifold) {
		for (auto& component : _components)
			component->OnCollisionStart(other, manifold);
	}

	void GameObject::OnCollisionEnd(ComponentPhysicsBody* other, b2Manifold* manifold) {
		for (auto& component : _components)
			component->OnCollisionEnd(other, manifold);
	}

	void GameObject::AddChild(std::shared_ptr<GameObject> p_object) {
		p_object->_parent = _self;
		_children.push_back(p_object);
	}

	void GameObject::AddComponent(std::shared_ptr<Component> p_component) {
		p_component->SetGameObject(_self);
		_components.push_back(p_component);
	}

	std::string GameObject::GetName() {
		return _name;
	}

	void GameObject::SetName(std::string p_name) {
		_name = p_name;
	}

	glm::mat4 GameObject::DeserializeTransform(rapidjson::Value& transformData) {
		auto pos = DeserializeVector(transformData["position"]);
		auto rot = DeserializeVector(transformData["rotation"]);
		auto scl = DeserializeVector(transformData["scale"]);

		return
			glm::translate(pos) *
			glm::mat4_cast(glm::quat(glm::radians(rot))) * 
			glm::scale(scl);
	}

	glm::vec3 GameObject::DeserializeVector(rapidjson::Value& vectorData) {
		assert(vectorData.IsArray() && "Trying to deserialize a vector from non-vector json value");
		assert(vectorData.Size() == 3 && "Trying to deserialize a vector from vector json value that doesn't have 3 elements (only 3d vectors supported ATM)");

		glm::vec3 ret{};
		for (int i = 0; i < vectorData.Size(); ++i)
			ret[i] = vectorData[i].GetFloat();

		return ret;
	}

	glm::vec3 GameObject::GetPosition() {
		return transform[3];
	}

	glm::quat GameObject::GetRotation() {
		const glm::vec3 scale = GetScale();
		return glm::quat_cast(glm::mat3(
			glm::vec3(transform[0]) / scale[0],
			glm::vec3(transform[1]) / scale[1],
			glm::vec3(transform[2]) / scale[2]));
	}

	glm::vec3 GameObject::GetEulerAngles() {
		return glm::degrees(ToEulerSafe(GetRotation()));
	}

	glm::vec3 GameObject::GetScale() {
		return glm::vec3{
			glm::length(glm::vec3(transform[0])),
			glm::length(glm::vec3(transform[1])),
			glm::length(glm::vec3(transform[2]))
		};
	}

	void GameObject::SetPosition(glm::vec3 position) {
		transform[3][0] = position[0];
		transform[3][1] = position[1];
		transform[3][2] = position[2];
	}

	void GameObject::SetRotation(glm::quat rotation) {
		transform =
			glm::translate(GetPosition()) * 
			glm::mat4_cast(rotation) * 
			glm::scale(GetScale());
	}

	void GameObject::SetEulerAngles(glm::vec3 eulerAngles) {
		SetRotation(ToQuaternionSafe(glm::radians(eulerAngles)));
	}

	void GameObject::SetScale(glm::vec3 scale) {
		glm::vec3 currScale = GetScale();
		currScale[0] = 1 / currScale[0];
		currScale[1] = 1 / currScale[1];
		currScale[2] = 1 / currScale[2];

		// we first undo the current scale, then apply the new one
		transform = glm::scale(scale) * glm::scale(currScale) * transform;
	}

	void GameObject::ResetTransform() {
		transform = glm::mat4(1);
	}
}
