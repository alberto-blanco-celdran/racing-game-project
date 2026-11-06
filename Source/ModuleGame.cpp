#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"

#include <algorithm>
#include <iostream>

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

ModuleGame::ModuleGame(Application* app, bool start_enabled)
	: Module(app, start_enabled)
{
}

ModuleGame::~ModuleGame()
{
}

// Load assets
bool ModuleGame::Start()
{
	LOG("Loading assets");

	App->renderer->camera.x = App->renderer->camera.y = 0;

	return true;
}

// Unload assets
bool ModuleGame::CleanUp()
{
	LOG("Unloading scene");

	return true;
}

// Update: draw background
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

void ModuleGame::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
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