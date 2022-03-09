#pragma once
#include "stdafx.h"
#include "TransformNode.h"  // scene graph transform node
#include "PhysObject.h"
#include "contact.h"
#include "CollisionGrid.h"

class Physics
{
	// a simple phyics system that moves objects around using update callbacks
	// in the RenderLib scenegraph system.
	std::vector<PhysicsObject*> objectList;
	std::vector<Collider*> staticColliders;
	glm::vec3 gravity;
	Node* sgNode;
	float cpuTimeLastStep;
	bool spatialH = true;
	int CollisionQu = 0;
	CollisionGrid<Collider*> *staticCollGrid;

	CollisionGrid<PhysicsObject*>* dynamicCollGrid;

	struct Pair 
	{
		PhysicsObject* c1; PhysicsObject* c2;
		Pair(PhysicsObject* first, PhysicsObject* second)
		{ 
			if (second < first) 
			{
				c1 = second; c2 = first;
			}
			else 
			{
				c1 = first; c2 = second;
			} 
		}
		uint64_t value() const { return((uint64_t)c1) << 32 | (uint64_t)c2; }
		bool operator == (const Pair& other) const 
		{
			return c1 == other.c1 && c2 == other.c2; 
		}
		bool operator < (const Pair& other) const 
		{
			return value() < other.value(); 
		} 
	};

public:
	Physics() 
	{
		staticCollGrid = new CollisionGrid<Collider*>(-15.0f, 15.0f, -15.0f, 15.0f, 64, 64);
		dynamicCollGrid = new CollisionGrid<PhysicsObject*>(-15.0f, 15.0f, -15.0f, 15.0f, 64, 64);
		
	}
	void SetGravity(const glm::vec3& grav) { gravity = grav; }
	glm::vec3 GetGravity() const { return gravity; }
	// update function - step all objects and update the scenegraph nodes
	void StepPhysics(float deltaT);
	// add body
	void AddPhysicsBody(TransformNode* sgNode, Collider *collider, const glm::vec3& pos, const glm::vec3& vel, float mass = 1.0f );
	// add static collider
	void AddStaticCollider(Collider* collider);
	// collide with world 
	bool CollideWithWorld(Collider* collider );
	bool CollideWithWorld(Collider* collider, std::vector<Contact>& contactList );
	int GetNumObjects() const { return objectList.size(); }
	float GetCPUTime() const { return cpuTimeLastStep; }
	int GetQueries() const { return CollisionQu; }
	void ChangeCollision() { spatialH = !spatialH; }
	bool GetColisionType() { return spatialH; }

};
