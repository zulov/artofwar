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
	int getDbID() override;
	void populate(db_resource* _dbResource);

	float getHealthBarSize() override;
	String& toMultiLineString() override;
	std::string getValues(int precision) override;
	static std::string getColumns();
	double collect(double collectSpeed);
	bool belowLimit();
	void up();
	void reduce();
private:
	db_resource* dbResource;
	int type;
	float amonut;

	static double hbMaxSize;
	int maxUsers;
	int users = 0;
};
