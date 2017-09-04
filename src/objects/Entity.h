#pragma once
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Math/Vector3.h>

#include <Urho3D/Scene/Component.h>
#include "ActionType.h"
#include "unit/aim/Aims.h"
#include "unit/ActionParameter.h"
#include "ObjectEnums.h"

using namespace Urho3D;

class Entity
{
public:
	Entity(Urho3D::Node* _node, ObjectType _type);
	virtual ~Entity();

	Urho3D::Node* getNode();

	bool isAlive();

	ObjectType getType();
	virtual int getSubTypeId();
	virtual void action(ActionType actionType, ActionParameter* parameter);
protected:
	Urho3D::Node* node;
	ObjectType type = ENTITY;
	bool alive;	

};
