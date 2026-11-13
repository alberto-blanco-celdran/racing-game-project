#pragma once

#include "Globals.h"
#include "Module.h"

#include "p2Point.h"

#include "raylib.h"
#include <vector>

class PhysBody;
class PhysicsEntity;
class Vehicle;

class ModuleGame : public Module
{
protected:
	Vehicle* player;

public:
	ModuleGame(Application* app, bool start_enabled = true);
	// ~ModuleGame();

	bool Start();
	update_status Update();
	bool CleanUp();

	// void OnCollision(PhysBody* bodyA, PhysBody* bodyB) override;

	void DestroyEntity(PhysicsEntity* entity);
	void DestroyAllEntities();

	bool debug = true; // TODO: change to 'false' before Release
	std::vector<PhysicsEntity*> entities;
};
