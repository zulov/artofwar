#pragma once
#include "database/db_strcut.h"
#include "objects/Entity.h"
#include "objects/Static.h"

class ResourceEntity :
	public Static
{
public:
	ResourceEntity(Vector3* _position, int id, int level);
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

	static float hbMaxSize;
	int maxUsers;
	int users = 0;
};
