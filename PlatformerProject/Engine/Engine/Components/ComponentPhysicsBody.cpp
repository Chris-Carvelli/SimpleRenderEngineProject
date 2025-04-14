#include "ComponentPhysicsBody.h"

#include "Engine/MyEngine.h"

ComponentPhysicsBody::~ComponentPhysicsBody() {
	if (_shape != nullptr)
	{
		delete _shape;
		_shape = nullptr;
	}

	if (_body != nullptr && _fixture != nullptr) {
		_body->DestroyFixture(_fixture);
		_fixture = nullptr;
	}
	if (_world != nullptr && _body != nullptr) {
		_world->DestroyBody(_body);
		_body = nullptr;
	}

	_world = nullptr;
}

void ComponentPhysicsBody::Init(rapidjson::Value& serializedData) {
	// get data needed for initialization
	int bodyType = serializedData["bodyType"].GetInt();
	bool isSensor = serializedData["isSensor"].GetBool();
	float density = serializedData["density"].GetFloat();
	glm::vec2 size = DeserializeVector2(serializedData["size"]);

	CreateBody(static_cast<b2BodyType>(bodyType), isSensor, size, density);
}

void ComponentPhysicsBody::CreateBody(b2BodyType bodyType, bool isSensor, glm::vec2 size, float density) {
	glm::vec2 pos = GetGameObject().lock()->GetPosition();
	auto engine = MyEngine::Engine::GetInstance();
	float physicsScale = engine->GetPhysicsScale();

	// world
	_world = engine->GetB2World();

	// shape
	{
		_shape = new b2PolygonShape();
		_shape->SetAsBox(size.x / physicsScale, size.y / physicsScale);
	}

	// body
	{
		b2BodyDef bodyDef;
		bodyDef.type = bodyType;
		bodyDef.position = b2Vec2(pos.x / physicsScale, pos.y / physicsScale);
		bodyDef.fixedRotation = true;

		_body = _world->CreateBody(&bodyDef);
	}

	// fixture
	{
		b2FixtureDef fixtureDef;
		fixtureDef.shape = _shape;
		fixtureDef.isSensor = isSensor;
		fixtureDef.density = density;

		_fixture = _body->CreateFixture(&fixtureDef);
	}

	engine->RegisterPhysicsComponent(this);
}


glm::vec2 ComponentPhysicsBody::DeserializeVector2(rapidjson::Value& vectorData) {
	assert(vectorData.IsArray() && "Trying to deserialize a vector from non-vector json value");
	assert(vectorData.Size() == 2 && "Trying to deserialize a vector from vector json value that doesn't have 2 elements");

	glm::vec2 ret{};
	for (int i = 0; i < vectorData.Size(); ++i)
		ret[i] = vectorData[i].GetFloat();

	return ret;
}

void ComponentPhysicsBody::addImpulse(glm::vec2 impulse) {
	b2Vec2 iForceV{ impulse.x,impulse.y };
	_body->ApplyLinearImpulse(iForceV, _body->GetWorldCenter(), true);
}

void ComponentPhysicsBody::addForce(glm::vec2 force) {
	b2Vec2 forceV{ force.x,force.y };
	_body->ApplyForce(forceV, _body->GetWorldCenter(), true);
}

void ComponentPhysicsBody::setPosition(glm::vec3 newPos) {
	auto engine = MyEngine::Engine::GetInstance();
	float physicsScale = engine->GetPhysicsScale();
	b2Vec2 newPosVec(newPos[0] / physicsScale, newPos[1] / physicsScale);
	_body->SetTransform(newPosVec, newPos[2]);
}

glm::vec2 ComponentPhysicsBody::getLinearVelocity() {
	b2Vec2 v = _body->GetLinearVelocity();
	return { v.x,v.y };
}

void ComponentPhysicsBody::setLinearVelocity(glm::vec2 velocity) {
	b2Vec2 v{ velocity.x, velocity.y };
	if (velocity != glm::vec2(0, 0)) {
		_body->SetAwake(true);
	}
	_body->SetLinearVelocity(v);
}