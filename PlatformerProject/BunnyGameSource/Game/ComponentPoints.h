#pragma once

#include "Engine/Component.h"

class ComponentPoints : public MyEngine::Component {
private:
	std::shared_ptr<MyEngine::GameObject> playerObject;
	int _bestPoints = 0;
	int _currentHeight;
	int _bestHeight;
	int _currentBest = 0;
	float _gameTime;
	bool _playerDead = false;
public:
	virtual void Init(rapidjson::Value& serializedData) override;
	void Update(float deltaTime) override;
	virtual void Render(sre::RenderPass& renderPass) override;
	int GetPoints() { return _bestPoints; }
	void SetGameTime(float gt) { _gameTime = gt; }
	void ResetPoints() { _currentBest = 0; _bestHeight = 0; }
	void ToggleDeath() { _playerDead = !_playerDead; }
	int GetHeight() { return _bestHeight; }
};