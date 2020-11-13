#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModulePhysics.h"
#include "p2Point.h"
#include "math.h"

#ifdef _DEBUG
#pragma comment( lib, "Box2D/libx86/Debug/Box2D.lib" )
#else
#pragma comment( lib, "Box2D/libx86/Release/Box2D.lib" )
#endif

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	world = NULL;
	mouse_joint = NULL;
	debug = true;
}

// Destructor
ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Start()
{
	LOG("Creating Physics 2D environment");

	world = new b2World(b2Vec2(GRAVITY_X, -GRAVITY_Y));
	world->SetContactListener(this);

	// needed to create joints like mouse joint
	b2BodyDef bd;
	ground = world->CreateBody(&bd);

	return true;
}

update_status ModulePhysics::PreUpdate()
{
	world->Step(1.0f / 60.0f, 6, 2);

	for(b2Contact* c = world->GetContactList(); c; c = c->GetNext())
	{
		if(c->GetFixtureA()->IsSensor() && c->IsTouching())
		{
			PhysBody* pb1 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			PhysBody* pb2 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			if(pb1 && pb2 && pb1->listener)
				pb1->listener->OnCollision(pb1, pb2);
		}
	}

	return UPDATE_CONTINUE;
}

void ModulePhysics::CreateWalls()
{
	int x = 0;
	int y = 0;
	int size = 48;
	int pinballWall[48] = {
		57, 771,
		57, 176,
		136, 32,
		672, 32,
		753, 179,
		753, 756,
		681, 756,
		681, 213,
		664, 191,
		648, 212,
		649, 770,
		507, 877,
		529, 880,
		526, 903,
		677, 788,
		785, 788,
		784, 166,
		692, 1,
		117, 0,
		25, 165,
		25, 787,
		180, 906,
		177, 884,
		199, 878
	};

	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* walls = world->CreateBody(&body);

	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[size];

	for (uint i = 0; i < size / 2; i++)
	{
		p[i].x = PIXEL_TO_METERS(pinballWall[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(pinballWall[i * 2 + 1]);
	}

	shape.CreateLoop(p, size / 2);

	b2FixtureDef fixture;
	fixture.shape = &shape;

	walls->CreateFixture(&fixture);
}

void ModulePhysics::CreateLeftSlingshot()
{
	int x = 0;
	int y = 0;
	int size = 16;
	int pinballSlingshot[16] = {
	142, 548,
	126, 554,
	104, 670,
	109, 686,
	123, 697,
	198, 738,
	208, 736,
	209, 726
	};

	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* leftSlingshot = world->CreateBody(&body);

	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[size];

	for (uint i = 0; i < size / 2; i++)
	{
		p[i].x = PIXEL_TO_METERS(pinballSlingshot[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(pinballSlingshot[i * 2 + 1]);
	}

	shape.CreateLoop(p, size / 2);

	b2FixtureDef fixture;
	fixture.shape = &shape;

	leftSlingshot->CreateFixture(&fixture);
}

void ModulePhysics::CreateRightSlingshot()
{
	int x = 0;
	int y = 0;
	int size = 16;
	int pinballSlingshot[16] = {
		560, 546,
		577, 556,
		598, 669,
		592, 686,
		576, 697,
		505, 736,
		496, 735,
		492, 726
	};

	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* leftSlingshot = world->CreateBody(&body);

	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[size];

	for (uint i = 0; i < size / 2; i++)
	{
		p[i].x = PIXEL_TO_METERS(pinballSlingshot[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(pinballSlingshot[i * 2 + 1]);
	}

	shape.CreateLoop(p, size / 2);

	b2FixtureDef fixture;
	fixture.shape = &shape;

	leftSlingshot->CreateFixture(&fixture);
}

void ModulePhysics::CreateBumper(int x, int y, int radius)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(radius);
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 1.0f;

	b->CreateFixture(&fixture);
}

PhysBody* ModulePhysics::CreateCircle(int x, int y, int radius)
{
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(radius);
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 1.0f;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = radius;

	return pbody;
}

PhysBody* ModulePhysics::CreateRectangle(int x, int y, int width, int height)
{
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);
	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = width * 0.5f;
	pbody->height = height * 0.5f;

	return pbody;
}

PhysBody* ModulePhysics::CreateRectangleSensor(int x, int y, int width, int height)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;
	fixture.isSensor = true;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = width;
	pbody->height = height;

	return pbody;
}

PhysBody* ModulePhysics::CreateChain(int x, int y, int* points, int size)
{
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[size / 2];

	for(uint i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
	}

	shape.CreateLoop(p, size / 2);

	b2FixtureDef fixture;
	fixture.shape = &shape;

	b->CreateFixture(&fixture);

	delete p;

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = 0;

	return pbody;
}

PhysBody* ModulePhysics::CreateLeftFlipper(int x1, int y1, int width, int height, int x2, int y2)
{
	// Filpper 
	b2BodyDef flipperDef;
	flipperDef.type = b2_dynamicBody;
	flipperDef.position.Set(PIXEL_TO_METERS(x1), PIXEL_TO_METERS(y1));

	b2Body* flipper = world->CreateBody(&flipperDef);
	b2PolygonShape box1;
	box1.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);


	b2FixtureDef fixture1;
	fixture1.shape = &box1;
	fixture1.density = 1.0f;

	flipper->CreateFixture(&fixture1);

	PhysBody* pbody1 = new PhysBody();
	pbody1->body = flipper;
	flipper->SetUserData(pbody1);
	pbody1->width = width * 0.5f;
	pbody1->height = height * 0.5f;
	
	// Anchor
	b2BodyDef anchorDef;
	anchorDef.type = b2_staticBody;
	anchorDef.position.Set(PIXEL_TO_METERS(x2), PIXEL_TO_METERS(y2));

	b2Body* anchor = world->CreateBody(&anchorDef);
	b2PolygonShape box2;
	box2.SetAsBox(PIXEL_TO_METERS(1), PIXEL_TO_METERS(1));

	
	
	b2FixtureDef fixture2;
	fixture2.shape = &box2;

	anchor->CreateFixture(&fixture2);

	b2RevoluteJointDef jointDef;
	jointDef.Initialize(anchor, flipper,(anchor->GetWorldCenter()));
	jointDef.bodyA = flipper;
	jointDef.bodyB = anchor;
	jointDef.localAnchorA.Set(PIXEL_TO_METERS(30), 0);
	jointDef.localAnchorB.Set(0, 0);

	world->CreateJoint(&jointDef);

	return pbody1;
}

PhysBody* ModulePhysics::CreateRightFlipper(int x1, int y1, int width, int height, int x2, int y2)
{
	// Filpper 
	b2BodyDef flipperDef;
	flipperDef.type = b2_dynamicBody;
	flipperDef.position.Set(PIXEL_TO_METERS(x1), PIXEL_TO_METERS(y1));

	b2Body* flipper = world->CreateBody(&flipperDef);
	b2PolygonShape box1;
	box1.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);


	b2FixtureDef fixture1;
	fixture1.shape = &box1;
	fixture1.density = 1.0f;

	flipper->CreateFixture(&fixture1);

	PhysBody* pbody1 = new PhysBody();
	pbody1->body = flipper;
	flipper->SetUserData(pbody1);
	pbody1->width = width * 0.5f;
	pbody1->height = height * 0.5f;

	// Anchor
	b2BodyDef anchorDef;
	anchorDef.type = b2_staticBody;
	anchorDef.position.Set(PIXEL_TO_METERS(x2), PIXEL_TO_METERS(y2));

	b2Body* anchor = world->CreateBody(&anchorDef);
	b2PolygonShape box2;
	box2.SetAsBox(PIXEL_TO_METERS(1), PIXEL_TO_METERS(1));



	b2FixtureDef fixture2;
	fixture2.shape = &box2;

	anchor->CreateFixture(&fixture2);

	b2RevoluteJointDef jointDef;
	jointDef.Initialize(anchor, flipper, (anchor->GetWorldCenter()));
	jointDef.bodyA = flipper;
	jointDef.bodyB = anchor;
	jointDef.localAnchorA.Set(PIXEL_TO_METERS(-30), 0);
	jointDef.localAnchorB.Set(0, 0);

	world->CreateJoint(&jointDef);

	return pbody1;
}

update_status ModulePhysics::PostUpdate()
{
	if(App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if(!debug)
		return UPDATE_CONTINUE;

	// Bonus code: this will iterate all objects in the world and draw the circles
	// You need to provide your own macro to translate meters to pixels
	for(b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		for(b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
		{
			switch(f->GetType())
			{
				// Draw circles ------------------------------------------------
				case b2Shape::e_circle:
				{
					b2CircleShape* shape = (b2CircleShape*)f->GetShape();
					b2Vec2 pos = f->GetBody()->GetPosition();
					App->renderer->DrawCircle(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), METERS_TO_PIXELS(shape->m_radius), 255, 255, 255);
				}
				break;

				// Draw polygons ------------------------------------------------
				case b2Shape::e_polygon:
				{
					b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
					int32 count = polygonShape->GetVertexCount();
					b2Vec2 prev, v;

					for(int32 i = 0; i < count; ++i)
					{
						v = b->GetWorldPoint(polygonShape->GetVertex(i));
						if(i > 0)
							App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);

						prev = v;
					}

					v = b->GetWorldPoint(polygonShape->GetVertex(0));
					App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);
				}
				break;

				// Draw chains contour -------------------------------------------
				case b2Shape::e_chain:
				{
					b2ChainShape* shape = (b2ChainShape*)f->GetShape();
					b2Vec2 prev, v;

					for(int32 i = 0; i < shape->m_count; ++i)
					{
						v = b->GetWorldPoint(shape->m_vertices[i]);
						if(i > 0)
							App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
						prev = v;
					}

					v = b->GetWorldPoint(shape->m_vertices[0]);
					App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
				}
				break;

				// Draw a single segment(edge) ----------------------------------
				case b2Shape::e_edge:
				{
					b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
					b2Vec2 v1, v2;

					v1 = b->GetWorldPoint(shape->m_vertex0);
					v1 = b->GetWorldPoint(shape->m_vertex1);
					App->renderer->DrawLine(METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y), METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y), 100, 100, 255);
				}
				break;
			}

			// TODO 1: If mouse button 1 is pressed ...
			// App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN
			// test if the current body contains mouse position
		}
	}

	// If a body was selected we will attach a mouse joint to it
	// so we can pull it around
	// TODO 2: If a body was selected, create a mouse joint
	// using mouse_joint class property


	// TODO 3: If the player keeps pressing the mouse button, update
	// target position and draw a red line between both anchor points

	// TODO 4: If the player releases the mouse button, destroy the joint

	return UPDATE_CONTINUE;
}

bool ModulePhysics::CleanUp()
{
	LOG("Destroying physics world");

	// Delete the whole physics world!
	delete world;

	return true;
}

void PhysBody::GetPosition(int& x, int &y) const
{
	b2Vec2 pos = body->GetPosition();
	x = METERS_TO_PIXELS(pos.x) - (width);
	y = METERS_TO_PIXELS(pos.y) - (height);
}

float PhysBody::GetRotation() const
{
	return RADTODEG * body->GetAngle();
}

bool PhysBody::Contains(int x, int y) const
{
	b2Vec2 p(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	const b2Fixture* fixture = body->GetFixtureList();

	while(fixture != NULL)
	{
		if(fixture->GetShape()->TestPoint(body->GetTransform(), p) == true)
			return true;
		fixture = fixture->GetNext();
	}

	return false;
}

int PhysBody::RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const
{
	int ret = -1;

	b2RayCastInput input;
	b2RayCastOutput output;

	input.p1.Set(PIXEL_TO_METERS(x1), PIXEL_TO_METERS(y1));
	input.p2.Set(PIXEL_TO_METERS(x2), PIXEL_TO_METERS(y2));
	input.maxFraction = 1.0f;

	const b2Fixture* fixture = body->GetFixtureList();

	while(fixture != NULL)
	{
		if(fixture->GetShape()->RayCast(&output, input, body->GetTransform(), 0) == true)
		{
			// do we want the normal ?

			float fx = x2 - x1;
			float fy = y2 - y1;
			float dist = sqrtf((fx*fx) + (fy*fy));

			normal_x = output.normal.x;
			normal_y = output.normal.y;

			return output.fraction * dist;
		}
		fixture = fixture->GetNext();
	}

	return ret;
}

void ModulePhysics::BeginContact(b2Contact* contact)
{
	PhysBody* physA = (PhysBody*)contact->GetFixtureA()->GetBody()->GetUserData();
	PhysBody* physB = (PhysBody*)contact->GetFixtureB()->GetBody()->GetUserData();

	if(physA && physA->listener != NULL)
		physA->listener->OnCollision(physA, physB);

	if(physB && physB->listener != NULL)
		physB->listener->OnCollision(physB, physA);
}