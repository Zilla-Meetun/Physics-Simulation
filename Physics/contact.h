#pragma once
#include "stdafx.h"

struct Contact
{
	// describes a contact in a collision
	glm::vec3 normal;
	float depth;
	float cr; // restitution
	float cf; // friction
};