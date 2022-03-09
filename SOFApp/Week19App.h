#pragma once
#include "stdafx.h"
#include "app.h"
#include "RenderLib.h"
#include "physics.h"

class Week19App : public SOF::App
{
	// geometry
	TexturedLit* floorQuad;
	TexturedLit* cuboid;

	// camera
	glm::vec3 eye;    
	glm::vec3 lookAt; 
	glm::vec3 up;     
	FPCam* camera;

	// Font plotter
	SOF::FontPlot *fontPlot;

	// scene graph
	Node* sceneGraphRoot;
	// root node for dynamic objects
	Node* dynamicObjects;
	// scene graph nodes for a projectile
	Node* projectileNode;
	Node* projectileNode2;

	// physics simulation
	Physics* physics;

	void Shoot(bool bHeavy);

public:
	virtual void Init();
	virtual void Draw();
	virtual void Update(float deltaT);
};