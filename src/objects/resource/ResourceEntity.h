#pragma once
#include "objects/Static.h"

struct db_resource;

class ResourceEntity :
	public Static
{
public:
	ResourceEntity(Vector3* _position, int id, int level);
	virtual ~ResourceEntity();
	void populate();

	float collect(float collectSpeed);

	bool belowLimit() const { return users < maxUsers; }
	void up() { ++users; }
	void reduce() { --users; }

	static std::string getColumns();

	int getDbID() override;
	float getMaxHpBarSize() override;
	String& toMultiLineString() override;
	std::string getValues(int precision) override;
private:
	db_resource* dbResource;
	int type;
	float amonut;

	int maxUsers;
	int users = 0;
};
