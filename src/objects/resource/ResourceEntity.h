#pragma once
#include "objects/Static.h"

struct db_resource;

class ResourceEntity :
	public Static
{
public:
	ResourceEntity(Vector3* _position, int id, int level, IntVector2& _bucketCords);
	virtual ~ResourceEntity();
	void populate();

	float collect(float collectSpeed);

	static std::string getColumns();


	bool belowCloseLimit() const override;
	int getDbID() override;
	bool isAlive() const override;
	float getMaxHpBarSize() override;
	float getHealthPercent() const override;
	String& toMultiLineString() override;
	std::string getValues(int precision) override;
private:
	bool hasFreeSpace() const;

	db_resource* dbResource;
	float amonut;
	unsigned char userPerCell = 1;
};
