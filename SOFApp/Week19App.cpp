#include "stdafx.h"
#include "Week19App.h"
#include "TexturedLit.h"
#include "Physics.h"
#include <random>

Node* Scene(Physics* physics);
float restScale = 1;
float frictScale = 1;
void Week19App::Init()
{
	// physics simulation
	physics = new Physics();
	physics->SetGravity(glm::vec3(0.0f, -4.0f, 0.0f));

	// setup the camera
	eye = glm::vec3(15.0f, 3.0f, 15.0f);
	lookAt = glm::vec3(0.0f, 0.0f, 0.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0); // y is up!
	//camera = new FPCam(eye, lookAt, physics, 2.0f);
	camera = new FPCam(eye, lookAt, physics, 3.0f);

	sceneGraphRoot = Scene(physics);

	// add a root node for dynamic object to the scene graph
	dynamicObjects = new GroupNode();
	sceneGraphRoot->AddChild(dynamicObjects);
	// a scenegraph node for a projectile:
	SphereGeometry sphereGeom = SphereGeometry(20);
	TexturedLit* sphereMesh = new TexturedLit(sphereGeom, "textures/grid.jpg");
	TexturedLit* sphereMesh2 = new TexturedLit(sphereGeom, "textures/stone.png");
	TransformNode* scale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(0.3f, 0.3f, 0.3f)));
	scale->AddChild(new GeometryNode(sphereMesh));
	projectileNode = scale;

	TransformNode* scale2 = new TransformNode(glm::scale(glm::mat4(), glm::vec3(0.6f, 0.6f, 0.6f)));
	scale2->AddChild(new GeometryNode(sphereMesh2));
	projectileNode2 = scale2;

	// add some dynamic spheres
	int nSpheres = 20;
	// random numbers:
	std::mt19937 rng = std::mt19937(std::random_device()());
	std::uniform_real_distribution<float> dist(0.0f, 1.0f);

	for (int i = 0; i < nSpheres; i++)
	{
		TransformNode* sphereNode = new TransformNode(glm::mat4());
		float radius = 0.3f;
		float mass = 1.0f;
		sphereNode->AddChild(projectileNode);
		dynamicObjects->AddChild(sphereNode);
		SphereCollider* collider = new SphereCollider(radius);
		collider->setFriction(0.5f*frictScale);
		collider->SetRestitution(0.4f * restScale);
		glm::vec3 pos = glm::vec3(-10.0f + 20.0f * dist(rng), 5.0f, -10.0f + 20.0f * dist(rng));
		physics->AddPhysicsBody(sphereNode, collider, pos, glm::vec3(), mass);

	}

	// font plotter
	fontPlot = new SOF::FontPlot("fonts/lucida32");
	// render states
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Week19App::Shoot(bool bHeavy)
{
	// this function will add a projectile to the physics system and a node to the scene graph

	// get the camera position and direction
	glm::vec3 camPos = camera->GetPos();
	glm::vec3 camDir = glm::normalize(camera->GetLookAt() - camera->GetPos());

	// Make a scene graph node for the new projectile
	TransformNode* projNode = new TransformNode(glm::mat4());
	float radius;
	float mass;
	if (bHeavy)
	{
		radius = 0.6f;
		mass = 10;
		projNode->AddChild(projectileNode2);
	}
	else
	{
		radius = 0.3f;
		mass = 3;
		projNode->AddChild(projectileNode);
	}
	dynamicObjects->AddChild(projNode);
	SphereCollider *collider = new SphereCollider(radius);

	if (bHeavy) 
	{
		collider->setFriction(frictScale);
		collider->SetRestitution(0.5f*restScale);
	}
	else 
	{
		collider->setFriction(frictScale);
		collider->SetRestitution(0.8f*restScale);
	}
	physics->AddPhysicsBody(projNode, collider, camPos, camDir * 10.0f, mass);
}

void Week19App::Draw()
{
	// camera
	glm::mat4 view;
	glm::mat4 proj;

	eye = camera->GetPos();
	lookAt = camera->GetLookAt();
	up = camera->GetUp();
	view = glm::lookAt(eye, lookAt, up);
	proj = glm::perspective(glm::radians(60.0f), (float)SOF::Window::GetWidth() / (float)SOF::Window::GetHeight(), 0.1f, 1000.0f);
	// Render the scene graph
	RenderVisitor rv(view, proj);
	rv.Traverse(sceneGraphRoot);
	// draw the HUD
	std::string hudString = "objects:";
	hudString =  hudString + std::to_string(physics->GetNumObjects());
	hudString = hudString + " CPU: ";
	hudString = hudString + std::to_string(physics->GetCPUTime());
	fontPlot->DrawString(50,100, hudString);

	hudString = "Collision Algorithm (C): ";
	if (physics->GetColisionType())
		hudString += "Spatial Hashing";
	else
		hudString += "Pair Managment";
	fontPlot->DrawString(50, 150, hudString);

	hudString = "Contacts per frame: ";
	hudString += std::to_string(physics->GetQueries());
	fontPlot->DrawString(50, 200, hudString);

	hudString = "Restitution scale (O/P): ";
	hudString += std::to_string(restScale);
	fontPlot->DrawString(700, 100, hudString);

	hudString = "Friction scale (U/I): ";
	hudString += std::to_string(frictScale);
	fontPlot->DrawString(700, 150, hudString);
}

void Week19App::Update(float deltaT)
{
	// update the physics
	physics->StepPhysics(deltaT);
	// Upadate the scene graph
	UpdateVisitor uv(deltaT);
	uv.Traverse(sceneGraphRoot);
	camera->Grav();
	if (SOF::Window::IsKeyDown(GLFW_KEY_W))
		camera->Move(10.0f*deltaT);
	if (SOF::Window::IsKeyDown(GLFW_KEY_S))
		camera->Move(-10.0f * deltaT);
	if (SOF::Window::IsKeyDown(GLFW_KEY_A))
		camera->Strafe(-10.0f * deltaT);
	if (SOF::Window::IsKeyDown(GLFW_KEY_D))
		camera->Strafe(10.0f * deltaT);

	if (SOF::Window::IsKeyDown(GLFW_KEY_LEFT))
		camera->Pan(40.0f * deltaT);
	if (SOF::Window::IsKeyDown(GLFW_KEY_RIGHT))
		camera->Pan(-40.0f * deltaT);
	if (SOF::Window::IsKeyDown(GLFW_KEY_UP))
		camera->LookUpDown(40.0f * deltaT);
	if (SOF::Window::IsKeyDown(GLFW_KEY_DOWN))
		camera->LookUpDown(-40.0f * deltaT);

	if (SOF::Window::IsKeyPressed(GLFW_KEY_O) && restScale<1)
		restScale += 0.1f;
	if (SOF::Window::IsKeyPressed(GLFW_KEY_P) && restScale>0)
		restScale -= 0.1f;
	if (SOF::Window::IsKeyPressed(GLFW_KEY_U) && frictScale < 1)
		frictScale += 0.1f;
	if (SOF::Window::IsKeyPressed(GLFW_KEY_I) && frictScale > 0)
		frictScale -= 0.1f;

	if (SOF::Window::IsKeyPressed(GLFW_KEY_C))
		physics->ChangeCollision();

	if (SOF::Window::IsKeyPressed(GLFW_KEY_SPACE))
	{
		if (SOF::Window::IsKeyDown(GLFW_KEY_H))
			Shoot(true);
		else
			Shoot(false);
	}
}


