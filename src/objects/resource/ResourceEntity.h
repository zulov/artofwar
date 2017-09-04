#pragma once
#include "objects/Entity.h"
#include "database/db_strcut.h"
#include "objects/Physical.h"

class ResourceEntity :
	public Physical
{
public:
	ResourceEntity(Vector3* _position, Urho3D::Node* _node);
	virtual ~ResourceEntity();
	int getSubTypeId() override;
	void populate(db_resource* _dbResource);
	bool isInGrandient();
	void setInGradinet(bool _inGradient);
	double getHealthBarSize() override;
	String *toMultiLineString() override;
private:
	String * name;
	db_resource * dbResource;
	int type;
	int amonut;
	bool inGradient = false;
	static double hbMaxSize;
};



