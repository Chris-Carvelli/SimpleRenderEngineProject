#include "ComponentRendererSprite.h"

#include "Engine/MyEngine.h"

#include "Engine/ResourceManager.h"

void ComponentRendererSprite::Init(rapidjson::Value& serializedData) {
	SetSprite(
		serializedData["atlas"].GetString(),
		serializedData["sprite"].GetString()
	);
}

void ComponentRendererSprite::SetSprite(std::string idAtlas, std::string idSprite)
{
	_atlas = MyEngine::ResourceManager::GetAtlas(idAtlas);
	_sprite = _atlas->get(idSprite);
}

void ComponentRendererSprite::Render(sre::RenderPass& renderPass) {
	auto gameObject = GetGameObject().lock();
	if (!gameObject)
		return;

	auto builder = sre::SpriteBatch::create();
	builder.addSprite(_sprite);
	auto batch = builder.build();
	renderPass.draw(batch, gameObject->transform);
}

void ComponentRendererSprite::SetSpritePosition(glm::vec2& pivot) {
	_sprite.setPosition(pivot);
}