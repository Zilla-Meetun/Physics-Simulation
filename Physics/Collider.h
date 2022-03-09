#pragma once
#include "stdafx.h"

typedef enum
{
	CTYPE_NONE,
	CTYPE_SPHERE,
	CTYPE_CAPSULE,
	CTYPE_PLANE,
	CTYPE_QUAD,
	CTYPE_SIZE
} COLL_TYPE;

class Collider
{
protected:
	COLL_TYPE collType;
	glm::vec3 pos;
	float cr; // coefficient of restitution
	float cf; // friction
public:
	const glm::vec3& GetObjectPos() const { return pos;  }
	void SetObjectPos(const glm::vec3& newPos) { pos = newPos; }
	float GetRestitution() const { return cr;  }
	void SetRestitution(float newCr) { cr = newCr; }
	float getFriction() const { return cf; }
	void setFriction(float newCf) { cf = newCf; }
	virtual void GetAABB(glm::vec3& vMin, glm::vec3& vMax) const = 0;
	Collider() : collType(CTYPE_NONE) {}
	Collider(COLL_TYPE type, float cr=1.0f, float cf=1.0f) : collType(type), cr(cr), cf(cf), pos(glm::vec3()) {}
	COLL_TYPE GetCollType() const { return collType; }
};

// all colliders are specified with at a position of 0,0,0. The position of the object (pos) is updated 
// from the physics simulation and added to all positions in the collision test
class SphereCollider : public Collider
{
	float radius;
public:
	SphereCollider(float radius) : Collider(CTYPE_SPHERE),  radius(radius) {};
	float GetRadius() const { return radius; }

	virtual void GetAABB(glm::vec3& vMin, glm::vec3& vMax) const 
	{
		vMin = pos - glm::vec3(radius, radius, radius);
		vMax = pos + glm::vec3(radius, radius, radius);
	}
};

class CapsuleCollider : public Collider
{
	glm::vec3 r0;
	glm::vec3 r1;
	float radius;
public:
	CapsuleCollider(const glm::vec3& r0, const glm::vec3& r1, float radius) : Collider(CTYPE_CAPSULE), r0(r0), r1(r1), radius(radius) {};
	glm::vec3 GetR0() { return r0; }
	glm::vec3 GetR1() { return r1; }
	float GetRadius() const { return radius; }

	virtual void GetAABB(glm::vec3& vMin, glm::vec3& vMax) const
	{
		vMin = r0;
		vMax = r0;
		if (r1.x < vMin.x) vMin.x = r1.x;
		if (r1.y < vMin.y) vMin.y = r1.y;
		if (r1.z < vMin.z) vMin.z = r1.z;
		if (r1.x > vMax.x) vMax.x = r1.x;
		if (r1.y > vMax.y) vMax.y = r1.y;
		if (r1.z > vMax.z) vMax.z = r1.z;
		vMin -= glm::vec3(radius, radius, radius);
		vMax += glm::vec3(radius, radius, radius);
	}
};

class PlaneCollider : public Collider
{
	glm::vec3 r;
	glm::vec3 n;
public:
	PlaneCollider(const glm::vec3& r, const glm::vec3& n) : Collider(CTYPE_PLANE), r(r), n(n) {};
	glm::vec3 GetPos() { return r; }
	glm::vec3 GetNorm() { return n; }

	virtual void GetAABB(glm::vec3& vMin, glm::vec3& vMax) const
	{
		vMin = glm::vec3(-1e8f, -1e8f, -1e8f);
		vMax = glm::vec3(1e8f, 1e8f, 1e8f);
	}
};

class QuadCollider : public Collider
{
	glm::vec3 r;
	glm::vec3 a; // a and b are the vectors along the sides of the quad
	glm::vec3 b; 
public:
	QuadCollider(const glm::vec3& r, const glm::vec3& a, const glm::vec3& b) : Collider(CTYPE_QUAD), r(r), a(a), b(b)
	{
	}
	const glm::vec3& GetR() const { return r; }
	const glm::vec3& GetA() const { return a; }
	const glm::vec3& GetB() const { return b; }

	virtual void GetAABB(glm::vec3& vMin, glm::vec3& vMax) const
	{
		glm::vec3 corners[4] = { r,r + a,r + b,r + a + b };
		vMin = glm::vec3(1e8f, 1e8f, 1e8f);
		vMax = glm::vec3(-1e8f, -1e8f, -1e8f);
		for (int i = 0; i < 4; i++) 
		{
			if (corners[i].x < vMin.x) vMin.x = corners[i].x;
			if (corners[i].y < vMin.y) vMin.y = corners[i].y;
			if (corners[i].z < vMin.z) vMin.z = corners[i].z;
			if (corners[i].x > vMax.x) vMax.x = corners[i].x;
			if (corners[i].y > vMax.y) vMax.y = corners[i].y;
			if (corners[i].z > vMax.z) vMax.z = corners[i].z;
		}
	} 

};