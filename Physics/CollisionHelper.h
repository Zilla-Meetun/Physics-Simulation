#pragma once
#include "Collider.h"
#include "contact.h"

class CollisionHelper
{
public:
	static bool Collision(Collider *c1, Collider *c2);
	static bool Collision(Collider *c1, Collider *c2, Contact& contact );
};