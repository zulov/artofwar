#pragma once
#include "Entity.h"
#include "db_strcut.h"

class ResourceEntity :
	public Entity
{
public:
	ResourceEntity(Vector3* _position, Urho3D::Node* _node);
	virtual ~ResourceEntity();
	ObjectType getType() override;
	virtual int getSubType() override;
	void populate(db_resource* dbResource);
	bool isInGrandient();
	void setInGradinet(bool _inGradient);
private:
	String * name;
	int type;
	int amonut;
	bool inGradient = false;
};



