#pragma once
#include "stdafx.h"
#include "collider.h"

class PhysicsObject
{
	glm::vec3 position;
	glm::vec3 velocity;
	Node* sgNode;
	Collider* collider;
	float mass;
public:
	PhysicsObject() : position(glm::vec3()), velocity(glm::vec3()), sgNode(nullptr) {}
	PhysicsObject(Node *sgNode, Collider *collider, float mass, const glm::vec3& position, const glm::vec3& velocity) : sgNode(sgNode), collider(collider), mass(mass), position(position), velocity(velocity) {}
	const glm::vec3& GetPosition() const { return position;  }
	const glm::vec3& GetVelocity() const { return velocity; }
	void SetPosition(const glm::vec3& pos) { position = pos; }
	void SetVelocity(const glm::vec3& vel) { velocity = vel; } 
	Collider* GetCollider() const { return collider; }
	float GetMass() const { return mass; }
	void SetMass( float newMass) { mass = newMass; }
	~PhysicsObject()
	{
		// delete the scene graph node - Node destructor should disconnect the graph
		delete sgNode;
		// delete the collider 
		delete collider;
	}
};

