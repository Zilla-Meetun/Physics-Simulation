#pragma once
#include "stdafx.h"
#include "UpdateCallback.h"
#include "TransformNode.h"
#include "PhysObject.h"

// scene graph update callback for physics objects

class PhysicsCallback : public UpdateCallback
{
	TransformNode* sgNode;
	PhysicsObject* physics;
public:
	PhysicsCallback(TransformNode* sgNode, PhysicsObject* physics) : sgNode(sgNode), physics(physics) {}
	virtual void operator()(float dt)
	{
		// copy the position from one to the other
		sgNode->SetTransform(glm::translate(glm::mat4(), physics->GetPosition()));
	}

};