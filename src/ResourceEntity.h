#pragma once
#include "Entity.h"
class ResourceEntity :
	public Entity
{
public:
	ResourceEntity(Vector3* _position, Urho3D::Node* _node);
	virtual ~ResourceEntity();
};

