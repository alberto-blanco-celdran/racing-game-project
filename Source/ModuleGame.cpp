#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"

#include <algorithm>

// Base class for everything in the scene that has a physics body.
class PhysicsEntity
{
protected:
	PhysicsEntity(PhysBody* _body)
		: body(_body), markedForDeletion(false)
	{
		_body->owner = this;
	}

public:
	virtual ~PhysicsEntity() = default;

	virtual void Update() = 0;

	PhysBody* body;
	bool markedForDeletion;
};

// Drivable vehicle.
class Vehicle : public PhysicsEntity
{
public:
	Vehicle(ModulePhysics* physics, int x, int y)
		: PhysicsEntity(physics->CreateRectangle(x, y, 8, 16))
	{
		friction = 2.0f;
		engineForce = 1.0f;
		steeringSpeed = 0.5f;
	}

	void Update() override
	{
		b2Vec2 velocity = body->body->GetLinearVelocity();
		b2Vec2 forward = body->body->GetWorldVector(b2Vec2(0.0f, 1.0f));
		b2Vec2 right = body->body->GetWorldVector(b2Vec2(1.0f, 0.0f));
		float rightDot = b2Dot(right, velocity);
		b2Vec2 rightVelocity = b2Vec2(right.x * rightDot, right.y * rightDot);
		float steerAngle = body->body->GetAngle() + steering * 20.0f;
		b2Vec2 dir = -b2Vec2(forward.x * cos(steerAngle) - forward.y * sin(steerAngle), forward.x * sin(steerAngle) + forward.y * cos(steerAngle));
		dir.Normalize();
		//body->body->SetAngularVelocity();
		body->body->ApplyForceToCenter(engineForce * throttle * dir - friction * rightVelocity, true);
		// TODO: calculate the forward angle rotated
		// TODO: rotate the car according to the new direction, not otherwise
		// TODO: change the point of impulse to the front, not the center, the effect is very different
		//body->body->ApplyTorque(steeringSpeed * steering, true);
		//body->body->SetAngularVelocity(steeringSpeed * steering);
		//body->body->ApplyForceToCenter(engineForce * throttle * front - friction * lateralVelocity, true);
	}

	// NOTE: consider have different cars of different pokemon, having different weights, handling, ...
	float friction;
	float engineForce;
	float steeringSpeed;
	float throttle;
	float steering;
};

ModuleGame::ModuleGame(Application* app, bool start_enabled)
	: Module(app, start_enabled)
{}

// Load assets.
bool ModuleGame::Start()
{
	LOG("Loading assets...");

	App->renderer->camera.x = App->renderer->camera.y = 0;

	player = new Vehicle(App->physics, 80, 400);
	entities.push_back(player);

	return true;
}

// Unload assets.
bool ModuleGame::CleanUp()
{
	LOG("Unloading scene...");

	return true;
}

// Update: draw background.
update_status ModuleGame::Update()
{
	// Toggle debug
	if (IsKeyPressed(KEY_F1)) debug = !debug;

	// Update & deletion
	for (PhysicsEntity* entity : entities)
	{
		if (entity->markedForDeletion) {
			DestroyEntity(entity);
			continue;
		}
		entity->Update();
	}

	// Car input
	if (IsKeyDown(KEY_UP)) {
		player->throttle = 1.0f;
	}
	else if (IsKeyDown(KEY_DOWN)) {
		player->throttle = -1.0f;
	}
	else {
		player->throttle = 0.0f;
	}

	if (IsKeyDown(KEY_LEFT)) {
		player->steering = -1.0f;
	}
	else if (IsKeyDown(KEY_RIGHT)) {
		player->steering = 1.0f;
	}
	else {
		player->steering = 0.0f;
	}

	return UPDATE_CONTINUE;
}

void ModuleGame::DestroyEntity(PhysicsEntity* entity)
{
	entities.erase(std::find(entities.begin(), entities.end(), entity));
	App->physics->DestroyBody(entity->body);
	delete entity;
}

void ModuleGame::DestroyAllEntities()
{
	for (PhysicsEntity* entity : entities)
	{
		App->physics->DestroyBody(entity->body);
		delete entity;
	}
	entities.clear();
}