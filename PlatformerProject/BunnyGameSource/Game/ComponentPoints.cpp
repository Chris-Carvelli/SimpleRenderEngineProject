#include "ComponentPoints.h"
#include "Engine/Components/ComponentPhysicsBody.h"
#include "Engine/MyEngine.h"
#include <iostream>

void ComponentPoints::Init(rapidjson::Value& serializedData) {
	playerObject = GetGameObject().lock();
	_bestPoints = 0;
	_bestHeight = playerObject->GetPosition()[1];
	_currentHeight = _bestHeight;
}

void ComponentPoints::Update(float deltaTime) {
	_currentHeight = playerObject->GetPosition()[1];

	if (_currentHeight > _bestHeight) {
		_currentBest += (_currentHeight - _bestHeight);
		_bestHeight = _currentHeight;
		//std::cout << points << std::endl;
	}

	if (_currentBest > _bestPoints) {
		_bestPoints = _currentBest;
	}

}

void ComponentPoints::Render(sre::RenderPass& renderPass) {
	if (!_playerDead) {
		ImGui::SetNextWindowSize(ImVec2(175, 90));
		ImGui::Begin("Points");
		ImGui::Text("High Score: %i", _bestPoints);
		ImGui::Text("Current Score: %i", _currentBest);
	}
	else {
		ImGui::SetNextWindowSize(ImVec2(565, 140));
		ImGui::Begin("You fell too far :(");
		ImGui::Text("Falling too far makes the bunny sad!");
		ImGui::Text("Press R to restart the game. Press Q or click the Exit button to quit the game.");
		if (ImGui::Button("Exit"))
		{
			SDL_Event exitEvent;
			exitEvent.type = SDL_QUIT;
			SDL_PushEvent(&exitEvent);
			MyEngine::Engine* engine = MyEngine::Engine::GetInstance();
			engine->ProcessEvents(exitEvent);
		}
		ImGui::Text("High Score: %i", _bestPoints);
		ImGui::Text("Current Score: %i", _currentBest);
	}
	ImGui::Text("Game Time: %i", (int)_gameTime);
	ImGui::End();
}