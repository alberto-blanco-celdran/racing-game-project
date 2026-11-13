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
		: PhysicsEntity(physics->CreateRectangle(x, y, 16, 32))
	{

	}

	void Update() override {};

	// NOTE: consider have different cars of different pokemon, having different weights, handling, ...
};

ModuleGame::ModuleGame(Application* app, bool start_enabled)
	: Module(app, start_enabled)
{}

// Load assets.
bool ModuleGame::Start()
{
	LOG("Loading assets...");

	App->renderer->camera.x = App->renderer->camera.y = 0;

	player = new Vehicle(App->physics, 640, 400);

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