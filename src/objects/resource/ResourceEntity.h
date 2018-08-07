#pragma once
#include "objects/static/Static.h"

struct db_resource;

class ResourceEntity :
	public Static
{
public:
	ResourceEntity(Urho3D::Vector3* _position, int id, int level, Urho3D::IntVector2& _bucketCords);
	virtual ~ResourceEntity();
	void populate();

	float collect(float collectSpeed);

	static std::string getColumns();

	int getDbID() override;
	float getMaxHpBarSize() override;
	float getHealthPercent() const override;
	Urho3D::String& toMultiLineString() override;
	std::string getValues(int precision) override;
private:

	db_resource* dbResource;
	float amonut;
};
