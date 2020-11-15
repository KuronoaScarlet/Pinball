#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"

class PhysBody;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);

public:
	p2List<PhysBody*> circles;
	p2List<PhysBody*> boxes;
	p2List<PhysBody*> ricks;
	p2List<PhysBody*> LeftFlipper;
	p2List<PhysBody*> RightFlipper;
	p2List<PhysBody*> Slingshot;
	p2List<PhysBody*> Bumpers;
	PhysBody* walls;
	PhysBody* rectangleSensor;
	PhysBody* circleSensor1;
	PhysBody* circleSensor2;
	PhysBody* circleSensor3;
	bool sensed;

	SDL_Texture* circle;
	SDL_Texture* box;
	SDL_Texture* rick;
	SDL_Texture* pinballMap;
	SDL_Texture* leftFlipper;
	SDL_Texture* rightFlipper;
	

	uint bonus_fx;
	p2Point<int> ray;
	bool rayOn;
	bool onScreen;

	//EasterEgg
	SDL_Texture* KNekro;
	bool easterEgg1 = false;
};
