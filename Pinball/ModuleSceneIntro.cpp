#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	circle = box = rick = NULL;
	ray_on = false;
	sensed = false;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	pinballMap = App->textures->Load("pinball/pinball.png");
	circle = App->textures->Load("pinball/ball.png"); 
	leftFlipper = App->textures->Load("pinball/leftFlipper.png");
	rightFlipper = App->textures->Load("pinball/rightFlipper.png");
	bonus_fx = App->audio->LoadFx("pinball/bonus.wav");

	//walls
	walls.add(App->physics->CreateWalls());
	walls.getLast()->data->listener = this;

	//Slingshots
	Slingshot.add(App->physics->CreateLeftSlingshot());
	Slingshot.getLast()->data->listener = this;
	Slingshot.add(App->physics->CreateRightSlingshot());
	Slingshot.getLast()->data->listener = this;
	

	//Bumpers
	
	Bumpers.add(App->physics->CreateBumper(207, 256, 64));
	Bumpers.getLast()->data->listener = this;
	Bumpers.add(App->physics->CreateBumper(354, 433, 64));
	Bumpers.getLast()->data->listener = this;
	Bumpers.add(App->physics->CreateBumper(505, 256, 64));
	Bumpers.getLast()->data->listener = this;
	
	//Ball
	circles.add(App->physics->CreateCircle(720, 600, 20));
	circles.getLast()->data->listener = this;

	//Left Flipper
	LeftFlipper.add(App->physics->CreateLeftFlipper(203, 905 ,120, 26, 201, 904));
	LeftFlipper.getLast()->data->listener = this;

	//Right Flipper
	RightFlipper.add(App->physics->CreateRightFlipper(499, 905, 120, 26, 501, 904));
	RightFlipper.getLast()->data->listener = this;

	//Sensors
	
	circleSensor = App->physics->CreateCircleSensor(720, 740, 30);

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	App->renderer->Blit(pinballMap, 0, 0, NULL);

	b2Vec2 force(0, -1000);
	b2Vec2 speed(0, 0);

	if(App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		ray_on = !ray_on;
		ray.x = App->input->GetMouseX();
		ray.y = App->input->GetMouseY();
	}

	if(App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		circles.add(App->physics->CreateCircle(App->input->GetMouseX(), App->input->GetMouseY(), 20));
		circles.getLast()->data->listener = this;
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
	{
		RightFlipper.getFirst()->data->body->ApplyTorque(2500.0f, true);
	}
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN)
	{
		LeftFlipper.getFirst()->data->body->ApplyTorque(-2500.0f, true);
	}
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN && circles.getFirst()->data->body->GetLinearVelocity() == speed)
	{
		circles.getFirst()->data->body->ApplyForce(force, circles.getFirst()->data->body->GetPosition(), true);
		rectangleSensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH, 10);
	}

	// Prepare for raycast ------------------------------------------------------
	
	iPoint mouse;
	mouse.x = App->input->GetMouseX();
	mouse.y = App->input->GetMouseY();
	int ray_hit = ray.DistanceTo(mouse);

	fVector normal(0.0f, 0.0f);

	// All draw functions ------------------------------------------------------
	p2List_item<PhysBody*>* c = circles.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y); //720, 600
		App->renderer->Blit(circle, x, y, NULL, 1.0f, c->data->GetRotation());
			
		c = c->next;
	}

	c = boxes.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(box, x, y, NULL, 1.0f, c->data->GetRotation());
		if(ray_on)
		{
			int hit = c->data->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);
			if(hit >= 0)
				ray_hit = hit;
		}
		c = c->next;
	}

	c = ricks.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(rick, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}

	c = LeftFlipper.getFirst();

	while (c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(leftFlipper, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}

	c = RightFlipper.getFirst();

	while (c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(rightFlipper, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}

	// ray -----------------
	if(ray_on == true)
	{
		fVector destination(mouse.x-ray.x, mouse.y-ray.y);
		destination.Normalize();
		destination *= ray_hit;

		App->renderer->DrawLine(ray.x, ray.y, ray.x + destination.x, ray.y + destination.y, 255, 255, 255);

		if(normal.x != 0.0f)
			App->renderer->DrawLine(ray.x + destination.x, ray.y + destination.y, ray.x + destination.x + normal.x * 25.0f, ray.y + destination.y + normal.y * 25.0f, 100, 255, 100);
	}

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	if (bodyB == rectangleSensor)
	{
		// Clear the ball
		App->audio->PlayFx(bonus_fx);
		circles.clear();

		// Create another one
		/*circles.add(App->physics->CreateCircle(720, 600, 20));
		circles.getLast()->data->listener = this;*/
	}
}
