#include "ComponentCamera.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "rapidjson/document.h"
#include "Engine/MyEngine.h"

void ComponentCamera::Init(rapidjson::Value& serializedData) {
	_camera = MyEngine::Engine::GetInstance()->GetCamera();

	float p_0 = 450;
	float p_1 = -1;
	float p_2 = 1;

	_camera->setOrthographicProjection(p_0, p_1, p_2);

	_camera->lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3{ 0,1,0 });
}

void ComponentCamera::Update(float delta) {
	
	if (auto gameObject = GetGameObject().lock())
	{
		glm::vec3 position = gameObject->GetPosition();
		glm::quat rotation = gameObject->GetRotation();
		_camera->setPositionAndRotation(position, glm::degrees(glm::eulerAngles(rotation)));
	}
}

