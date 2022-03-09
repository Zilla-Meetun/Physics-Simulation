#pragma once
#include "stdafx.h"
#include "physics.h"
#include <cmath>

class FPCam // first person camera
{
	glm::vec3 pos;
	glm::vec3 look;
	float theta, phi;
	glm::vec3 up;
	glm::vec3 direction;
	glm::vec3 side;

	Physics* physics;
	SphereCollider* collider;
	
	void updatevectors()
	{
		direction = glm::vec3(1, 0, 0);
		side = glm::vec3(0, 0, 1);
		// rotate direction and side about up by theta
		glm::mat3 rotMat = glm::rotate(glm::mat4(), glm::radians(theta), up);
		direction = rotMat * direction;
		side = rotMat * side;
		// now rotate direction about side by phi
		rotMat = glm::rotate(glm::mat4(), glm::radians(phi), side);
		direction = rotMat * direction;
	}
public:
	FPCam(const glm::vec3& pos, const glm::vec3& look, Physics* physics, float collisionRadius) : pos(pos), look(look), up(glm::vec3(0, 1, 0)), physics(physics)
	{
		glm::vec3 r = glm::normalize(look - pos);
		theta = glm::degrees(atan2(-r.z, r.x));
		phi = glm::degrees(asin(r.y));
		collider = new SphereCollider(collisionRadius);
		collider->setFriction(0.8f);
		updatevectors();
	}

	const glm::vec3& GetPos() const { return pos; }
	const glm::vec3& GetLookAt() const { return look; }
	const glm::vec3& GetUp() const { return up; }

	void Grav()
	{
		
	}

	void Pan(float delta)
	{
		theta += delta;
		if (theta < 0.0f)
			theta += 360.0f;
		if (theta > 360.0f)
			theta -= 360.0f;
		updatevectors();
		look = pos + direction;
	}

	void LookUpDown(float delta)
	{
		phi += delta;
		if (phi < -80.0f)
			phi = -80.0f;
		if (phi > 80.0f)
			phi = 80.0f;
		updatevectors();
		look = pos + direction;
	}

	void Move(float deltaMove)
	{
		// move the camera in the direction it's looking in, with the y component taken out 
		glm::vec3 floorDirection = direction;
		floorDirection.y = 0.0f;
		floorDirection = glm::normalize(floorDirection);
		// calculate new position
		glm::vec3 newPos = pos + floorDirection*deltaMove;
		// collision tests go here
		collider->SetObjectPos(newPos);
		
		std::vector<Contact> contactList;
		if (physics->CollideWithWorld(collider, contactList))
		{
			for (Contact c : contactList)
			{
				newPos += c.normal * c.depth;
			}
		}
		// update positions
		pos = newPos;
		look = pos + direction;
	}

	void Strafe(float deltaMove)
	{
		glm::vec3 floorSide = side;
		floorSide.y = 0.0f;
		floorSide = glm::normalize(floorSide);
		glm::vec3 newPos = pos + floorSide * deltaMove;
	
		collider->SetObjectPos(newPos);

		std::vector<Contact> contactList;
		if (physics->CollideWithWorld(collider, contactList))
		{
			for (Contact c : contactList)
			{
				newPos += c.normal * c.depth;
			}
		}
		// update positions
		pos = newPos;
		look = pos + direction;
	}
};