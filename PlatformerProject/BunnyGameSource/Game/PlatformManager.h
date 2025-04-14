#pragma once

#include "Engine/Component.h"
#include "Engine/GameObject.h"

class PlatformManager : public MyEngine::Component {
    virtual void Init(rapidjson::Value& serializedData) override;
    virtual void Update(float) override;
public:

    std::vector<MyEngine::GameObject*> objectHolder = {};
    void CleanupPlatforms(); // New method to cleanup platforms

private:
    float _playerMaxHeight = 0;
    int platformCounter = 0;
    int resetCounter = 0;
    std::weak_ptr<MyEngine::GameObject> _platformsParent; // Parent object for all platforms
};