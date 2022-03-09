#include "stdafx.h"
#include "physics.h"
#include "PhysCallback.h"
#include "CollisionHelper.h"
#include "CollisionGrid.h"
#include "timer.h"
#include <set>

void Physics::StepPhysics(float deltaT)
{
	SOF::Timer timer;
	timer.Reset();
	
		CollisionQu = 0;
	int tmpC = 0;
	auto it = objectList.begin();
	while (it != objectList.end())
	{
		PhysicsObject* pObj = *it;
		pObj->SetPosition(pObj->GetPosition() + pObj->GetVelocity() * deltaT);
		pObj->SetVelocity(pObj->GetVelocity() + gravity * deltaT);
		if (pObj->GetCollider() != nullptr)
			pObj->GetCollider()->SetObjectPos(pObj->GetPosition());
		
		pObj->GetPosition();

		if (pObj->GetPosition().x > 20 || pObj->GetPosition().z > 20 || pObj->GetPosition().x < -20 || pObj->GetPosition().z < -20)
		{
			it = objectList.erase(it);
			delete pObj;
		}
		else 
		{
			++it;
		}
	}
	// dynamic vs static collision
	for (auto pObj : objectList)
	{
		std::vector<Contact> contactList;
		if (CollideWithWorld(pObj->GetCollider(), contactList))
		{
			for (Contact c : contactList) {
				// resolve collision here
				tmpC++;
				float cr = c.cr;
				float v = glm::dot(pObj->GetVelocity(), c.normal);
				if (v < 0.0f) {
					pObj->SetVelocity(pObj->GetVelocity() - c.normal * (1.0f + cr) * v);
					// apply friction
					float cf = c.cf;
					// first calculate velocity parallel to the contact surface
					glm::vec3 vParallel = pObj->GetVelocity() - c.normal * glm::dot(pObj->GetVelocity(), c.normal);
					if (glm::dot(vParallel, vParallel) > 0.0f)
					{
						float collImpulse = -(1.0f + cr) * v;
						// remember v is negative
						float fricImpulse = collImpulse * cf;
						if (fricImpulse > glm::length(vParallel))
							pObj->SetVelocity(pObj->GetVelocity() - vParallel);
						else
							pObj->SetVelocity(pObj->GetVelocity() - glm::normalize(vParallel) * fricImpulse);
					}
				}
			}		
		}
	}
	if (spatialH) {
		// dynamic vs dynamic collision
		if (objectList.size() > 1)
		{
			for (int i = 0; i < objectList.size() - 1; i++)
			{
				PhysicsObject* pObj1 = objectList[i];
				for (int j = i + 1; j < objectList.size(); j++)
				{
					PhysicsObject* pObj2 = objectList[j];
					Contact c;
					float mu = pObj1->GetMass() / pObj2->GetMass(); // mass ratio

					if (CollisionHelper::Collision(pObj1->GetCollider(), pObj2->GetCollider(), c))
					{
						tmpC++;
						// resolve collision
						if (glm::dot(pObj1->GetVelocity() - pObj2->GetVelocity(), c.normal) < 0.0f)
						{
							float cr = c.cr;
							// collision
							float v1 = glm::dot(pObj1->GetVelocity(), c.normal);
							float v2 = glm::dot(pObj2->GetVelocity(), c.normal);
							// remove components in collision direction
							pObj1->SetVelocity(pObj1->GetVelocity() - c.normal * v1);
							pObj2->SetVelocity(pObj2->GetVelocity() - c.normal * v2);
							// calculate new components
							float v1p, v2p;
							v1p = (1.0f / (1.0f + mu)) * ((mu - cr) * v1 + (1.0f + cr) * v2);
							v2p = (1.0f / (1.0f + mu)) * (mu * (1.0f + cr) * v1 + (1.0f - mu * cr) * v2);
							// add components in collision direction
							pObj1->SetVelocity(pObj1->GetVelocity() + c.normal * v1p);
							pObj2->SetVelocity(pObj2->GetVelocity() + c.normal * v2p);
						}
					}
				}
			}
		}
	}
	else {
		std::set<Pair> collPairs;
		// first add all the moving objects to the dynamic broadphase
		dynamicCollGrid->Clear();
		for (auto o : objectList) {
			glm::vec3 vMin, vMax;
			o->GetCollider()->GetAABB(vMin, vMax);
			dynamicCollGrid->AddCollider(o, vMin.x, vMax.x, vMin.z, vMax.z);
		}
		// now query the grid for each object and add pairs
		std::vector<PhysicsObject*> colliders;
		for (auto o : objectList)
		{
			glm::vec3 vMin, vMax;
			o->GetCollider()->GetAABB(vMin, vMax);
			colliders.clear();
			dynamicCollGrid->GetColliderList(vMin.x, vMax.x, vMin.z, vMax.z, colliders);
			for (auto c : colliders)
			{
				if (c != o)
				{
					Pair p(c, o);
					collPairs.insert(p);
				}
			}
		}
		for (Pair p : collPairs)
		{
			PhysicsObject* pObj1 = p.c1;
			PhysicsObject* pObj2 = p.c2;
			Contact c;
			float mu = pObj1->GetMass() / pObj2->GetMass(); // mass ratio

			if (CollisionHelper::Collision(pObj1->GetCollider(), pObj2->GetCollider(), c))
			{
				tmpC++;
				// resolve collision
				if (glm::dot(pObj1->GetVelocity() - pObj2->GetVelocity(), c.normal) < 0.0f)
				{
					float cr = c.cr;
					// collision
					float v1 = glm::dot(pObj1->GetVelocity(), c.normal);
					float v2 = glm::dot(pObj2->GetVelocity(), c.normal);
					// remove components in collision direction
					pObj1->SetVelocity(pObj1->GetVelocity() - c.normal * v1);
					pObj2->SetVelocity(pObj2->GetVelocity() - c.normal * v2);
					// calculate new components
					float v1p, v2p;
					v1p = (1.0f / (1.0f + mu)) * ((mu - cr) * v1 + (1.0f + cr) * v2);
					v2p = (1.0f / (1.0f + mu)) * (mu * (1.0f + cr) * v1 + (1.0f - mu * cr) * v2);
					// add components in collision direction
					pObj1->SetVelocity(pObj1->GetVelocity() + c.normal * v1p);
					pObj2->SetVelocity(pObj2->GetVelocity() + c.normal * v2p);
				}
			}
		}
	}
	CollisionQu = tmpC;
	cpuTimeLastStep = timer.Elapsed();
}
void Physics::AddPhysicsBody(TransformNode* sgNode, Collider* collider, const glm::vec3& pos, const glm::vec3& vel, float mass)
{
	// create the physics object and add it to the list
	PhysicsObject* obj = new PhysicsObject(sgNode, collider, mass, pos, vel);
	objectList.push_back(obj);
	// create an update callback for the scenegraph transform, and hook it up
	PhysicsCallback* callback = new PhysicsCallback( sgNode, obj );
	sgNode->SetUpdateCallback(callback);
}

void Physics::AddStaticCollider(Collider* c)
{
	staticColliders.push_back(c);
	glm::vec3 vMin, vMax;
	c->GetAABB(vMin, vMax);
	staticCollGrid->AddCollider(c, vMin.x, vMax.x, vMin.z, vMax.z);

}

bool Physics::CollideWithWorld(Collider* c )
{
	std::vector<Contact> cList;
	return CollideWithWorld(c, cList);
}

bool Physics::CollideWithWorld(Collider* c, std::vector<Contact>& contactList )
{
	
	contactList.clear();
	
	glm::vec3 vMin, vMax;
	c->GetAABB(vMin, vMax);
	std::vector<Collider*> colliderList;
	staticCollGrid->GetColliderList(vMin.x, vMax.x, vMin.z, vMax.z, colliderList);
	for (Collider* cStatic : colliderList)
	{
		Contact contact;
		if (CollisionHelper::Collision(c, cStatic, contact))
		{
			contactList.push_back(contact);
			
		}
	}
	return contactList.size() != 0;
}

