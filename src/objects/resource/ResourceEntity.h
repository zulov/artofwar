#pragma once
#include "objects/static/Static.h"
#include "scene/load/dbload_container.h"

struct db_resource;

class ResourceEntity : public Static {
public:
	ResourceEntity(Urho3D::Vector3& _position, int id, int level, int mainCell);
	virtual ~ResourceEntity();
	void populate() override;

	float collect(float collectSpeed);
	void load(dbload_resource_entities* resource);

	static std::string getColumns();

	char getPlayer() const override { return -1; }

	int getDbID() override;
	float getMaxHpBarSize() override;
	float getHealthPercent() const override;
	Urho3D::String toMultiLineString() override;
	std::string getValues(int precision) override;
	void action(char id, const ActionParameter& parameter) override;
	ObjectType getType() const override;
private:
	Urho3D::String getBarMaterialName() override;
	db_resource* dbResource;
	float amonut;
};
