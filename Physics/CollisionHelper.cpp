#include "stdafx.h"
#include "CollisionHelper.h"
#include "GeomUtils.h"

bool CollisionHelper::Collision(Collider *c1, Collider *c2)
{
	Contact c;
	return Collision(c1, c2, c);
}

bool CollisionHelper::Collision(Collider *c1, Collider *c2, Contact& contact)
{
	if (c2->GetCollType() < c1->GetCollType())
	{
		Collider *swap = c1;
		c1 = c2;
		c2 = swap;
	}

	if (c1->GetCollType() == CTYPE_SPHERE && c2->GetCollType() == CTYPE_SPHERE)
	{
		SphereCollider *sc1 = (SphereCollider*)c1;
		SphereCollider *sc2 = (SphereCollider*)c2;
		glm::vec3 pos1, pos2;
		pos1 = sc1->GetObjectPos();
		pos2 = sc2->GetObjectPos();
		contact.normal = glm::normalize(pos1 - pos2);
		contact.depth = sc1->GetRadius() + sc2->GetRadius() - GeomUtils::Distance(pos1, pos2);
		contact.cr = (c1->GetRestitution() + c2->GetRestitution()) * 0.5f;
		contact.cf = (c1->getFriction() + c2->getFriction()) * 0.5f;
		return contact.depth > 0.0f;
	}
	else if (c1->GetCollType() == CTYPE_SPHERE && c2->GetCollType() == CTYPE_CAPSULE)
	{
		SphereCollider *sc = (SphereCollider*)c1;
		CapsuleCollider *cc = (CapsuleCollider*)c2;
		
		float dist = GeomUtils::PointToFiniteLineDistance(cc->GetR0(), cc->GetR1(), sc->GetObjectPos());
		glm::vec3 poin = GeomUtils::NearestPointOnFiniteLine(cc->GetR0(), cc->GetR1(), sc->GetObjectPos());
		contact.depth = (sc->GetRadius() + cc->GetRadius()) - dist;
		contact.normal = glm::normalize(sc->GetObjectPos() - poin);
		contact.cr = (c1->GetRestitution() + c2->GetRestitution()) * 0.5f;
		contact.cf = (c1->getFriction() + c2->getFriction()) * 0.5f;
		// spheere-capsule test here
		return contact.depth > 0.0f;
	}
	
	
	else if (c1->GetCollType() == CTYPE_SPHERE && c2->GetCollType() == CTYPE_PLANE)
	{
		SphereCollider* sc = (SphereCollider*)c1;
		PlaneCollider* pc = (PlaneCollider*)c2;
		// sphere-plane test here
		glm::vec3 spherePos = sc->GetObjectPos();
		float distance = GeomUtils::PointToPlaneDistance(pc->GetPos(), pc->GetNorm(), spherePos);
		contact.normal = pc->GetNorm();
		contact.depth = sc->GetRadius() - distance;
		contact.cr = (c1->GetRestitution() + c2->GetRestitution()) * 0.5f;
		contact.cf = (c1->getFriction() + c2->getFriction()) * 0.5f;
		return contact.depth > 0.0f;
	}
	else if (c1->GetCollType() == CTYPE_SPHERE && c2->GetCollType() == CTYPE_QUAD)
	{
		SphereCollider* sc = (SphereCollider*)c1;
		QuadCollider* qc = (QuadCollider*)c2;
		// sphere-quad test here
		glm::vec3 pos = sc->GetObjectPos();
		glm::vec3 nearPos = GeomUtils::NearestPointOnQuad(qc->GetR(), qc->GetA(), qc->GetB(), pos);
		float distance = GeomUtils::Distance(pos, nearPos);
		contact.depth = sc->GetRadius() - distance;
		contact.normal = glm::normalize(pos - nearPos);
		contact.cr = (c1->GetRestitution() + c2->GetRestitution()) * 0.5f;
		contact.cf = (c1->getFriction() + c2->getFriction()) * 0.5f;
		return contact.depth > 0.0f;
	}
	return false;
}