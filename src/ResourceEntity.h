#pragma once
#include "Entity.h"
#include "db_strcut.h"
#include "Physical.h"

class ResourceEntity :
	public Physical
{
public:
	ResourceEntity(Vector3* _position, Urho3D::Node* _node);
	virtual ~ResourceEntity();
	virtual int getSubType() override;
	void populate(db_resource* dbResource);
	bool isInGrandient();
	void setInGradinet(bool _inGradient);
private:
	String * name;
	int type;
	int amonut;
	bool inGradient = false;
	static double hbMaxSize;
};



