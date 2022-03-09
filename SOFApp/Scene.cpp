#include "stdafx.h"
#include "RenderLib.h"
#include "physics.h"
#include <random>

void BuildWall(GeometryNode* geom, Node* parent, Physics* physics, float cr, const glm::vec3& pos, float width, float height, float yAngle)
{
	// geom is a textured lit object made from a unit quad centred on the origin in the xz plane
	// parent is the scene graph node we want to add the wall to
	// the wall is defined by r, a, b - r one vertex, a, b vectors along the width/height
	// assume the bottom of the wall is at y = 0
	// and the pos is at the bottom of the wall half way along the width
	glm::mat4 wall = glm::mat4();
	wall = glm::translate(wall, pos + glm::vec3(0.0f, height * 0.5f, 0.0f));
	wall = glm::rotate(wall, glm::radians(yAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	wall = glm::rotate(wall, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	wall = glm::scale(wall, glm::vec3(width, 1, height));
	TransformNode* walltrans = new TransformNode(wall);
	parent->AddChild(walltrans);
	walltrans->AddChild(geom);
	// work out the vectors for the plane collision
	glm::vec4 dir = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	dir = glm::rotate(glm::mat4(), glm::radians(yAngle), glm::vec3(0, 1, 0) ) * dir;

	glm::vec3 r, a, b;
	a = dir * width;
	b = glm::vec3(0, height, 0);
	r = pos - a * 0.5f;

	QuadCollider* col = new QuadCollider(r, a, b);
	col->SetRestitution(cr);
	col->setFriction(0.9f);
	physics->AddStaticCollider(col);
}
void MakeCapsule(Node* parent, Physics* physics, float cr, const glm::vec3& pos, float radius, float height)
{
	CylinderGeometry cil(20, height);
	SphereGeometry sph(20);
	TexturedLit* mid = new TexturedLit(cil, "textures/marble.png");
	TexturedLit* end = new TexturedLit(sph, "textures/marble.png");
	TransformNode* tn = new TransformNode(glm::translate(glm::mat4(), pos + glm::vec3(0, height+radius, 0)));
	
	glm::vec3 hei = glm::vec3(0, height, 0);
	glm::vec3 radi = glm::vec3(0, radius, 0);

	glm::mat4 sScale, cScale, top, bottom = glm::mat4();
	sScale = glm::scale(sScale, glm::vec3(radius, radius, radius));
	cScale = glm::scale(cScale, glm::vec3(radius, 1, radius));
	
	top = glm::translate(sScale, hei+radi);
	bottom = glm::translate(sScale, -hei+radi);
	
	TransformNode* cbase = new TransformNode(glm::translate(cScale, radi));
	TransformNode* up = new TransformNode(top);
	TransformNode* sbase = new TransformNode(bottom);

	sbase->AddChild(new GeometryNode(end));
	up->AddChild(new GeometryNode(end));
	cbase->AddChild(new GeometryNode(mid));

	
	tn->AddChild(sbase);
	tn->AddChild(up);
	tn->AddChild(cbase);
	parent->AddChild(tn);

	glm::vec3 r0, r1;
	r0 = pos + hei*2.0f +radi*2.0f;
	r1 = pos + - radi;

	CapsuleCollider* col = new CapsuleCollider(r0, r1, radius);
	col->SetRestitution(cr);
	col->setFriction(0.9f);
	physics->AddStaticCollider(col);

}

Node* Scene( Physics* physics)
{
	QuadGeometry quad(10);
	std::vector<TexturedLit*> floors;
	floors.push_back(new TexturedLit(quad, "textures/marble.png"));
	floors.push_back(new TexturedLit(quad, "textures/brick.jpg"));
	floors.push_back(new TexturedLit(quad, "textures/stone.png", "textures/stonenorms.png"));
	floors.push_back(new TexturedLit(quad, "textures/wood_diffuse.jpg", "textures/wood_normal.png"));
	
	
	LightNode* light = new LightNode(glm::vec3(0, 50, 0), glm::vec4(1, 1, 1, 1), 0.1, 0.9, 0.4, 10, 50);

	// scale the floor and add 
	
	for (int i = -1; i < 3; i++ ) {
		TransformNode* floorScale = new TransformNode(glm::scale(glm::mat4(), glm::vec3(10, 40, 40)));
		TransformNode* floorTrans = new TransformNode(glm::translate(glm::mat4(), glm::vec3(i * 10, 0, 0)));
		
		light->AddChild(floorTrans);
		floorTrans->AddChild(floorScale);
		floorScale->AddChild(new GeometryNode(floors[i+1]));
		QuadCollider* f = new QuadCollider(glm::vec3(i*10,20,20), glm::vec3(10,0,0), glm::vec3(0,0,40));
		f->setFriction((1+i) * 0.25f);
		MakeCapsule(light, physics, 1, glm::vec3(10*i, 0, 0), 1, .5);
	}
	// walls
	QuadGeometry wallQuad(20);
	TexturedLit* wall = new TexturedLit(wallQuad, "textures/brick.jpg", "textures/bricknorms.jpg");
	GeometryNode* wallGeom = new GeometryNode(wall);

	//


	glm::vec3 positions[] = { glm::vec3(0.0f,0.0f,20.0f), glm::vec3(0.0f,0.0f,-20.0f), glm::vec3(20.0f, 0.0f, 0.0f), glm::vec3(-20.0f, 0.0f, 0.0f) };
	float widths[] = { 40,40,40,40 };
	float rotations[] = { 0,0,90,90 };

	for (int i = 0; i < sizeof(widths) / sizeof(float); i++)
	{
		float cr = 1.0f;
		if (i < 10)
			cr = 0.5f;
		BuildWall(wallGeom, light, physics, cr, positions[i], widths[i], 10.0f, rotations[i]);
	}
	// add a plane collider for the floor
	PlaneCollider* pc = new PlaneCollider(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	pc->setFriction(0.1f);
	physics->AddStaticCollider(pc);

	// add 100 static spheres on a regular grid
	SphereGeometry sg(20);
	TexturedLit* staticSphere = new TexturedLit(sg, "textures/marble.png");
	GeometryNode* staticSphereGeom = new GeometryNode(staticSphere);

	for (int i = 0; i < 10; i+=3)
	{
		float x = -9.0f + 18.0f * (float)i / 9.0f;
		for (int j = 0; j < 10; j+=3)
		{
			float z = -9.0f + 18.0f * (float)j / 9.0f;

			TransformNode* tn = new TransformNode(glm::scale(glm::translate(glm::mat4(), glm::vec3(x, 1.0f, z)),glm::vec3(0.6f,0.6f,0.6f)));
			light->AddChild(tn);
			tn->AddChild(staticSphereGeom);
			SphereCollider* sc = new SphereCollider(0.6f);
			sc->SetObjectPos(glm::vec3(x, 1.0f, z));
			sc->SetRestitution(0.2f);
			sc->setFriction(0.2f);
			physics->AddStaticCollider(sc);
		}
	}
		
	return light;
}
