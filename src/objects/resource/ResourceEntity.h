#pragma once
#include "objects/Entity.h"
#include "database/db_strcut.h"
#include "objects/Static.h"

class ResourceEntity :
	public Static
{
public:
	ResourceEntity(Vector3* _position, int id);
	virtual ~ResourceEntity();
	int getID() override;
	void populate(db_resource* _dbResource);

	double getHealthBarSize() override;
	String* toMultiLineString() override;
	double collect(double collectSpeed);
	bool belowLimit();
	void up();
	void reduce();
private:
	String* name;
	db_resource* dbResource;
	int type;
	double amonut;
	bool inGradient = false;
	static double hbMaxSize;
	int maxUsers;
	int users = 0;
};
