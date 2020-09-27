#pragma once
#include "objects/static/Static.h"
#include "scene/load/dbload_container.h"

enum class ResourceActionType : char;
struct db_resource;

class ResourceEntity : public Static {
public:
	ResourceEntity(Urho3D::Vector3& _position, int id, int level, int mainCell);
	virtual ~ResourceEntity() = default;
	void populate() override;

	float collect(float collectSpeed);
	ResourceEntity* load(dbload_resource_entities* resource);

	static std::string getColumns();

	char getPlayer() const override { return -1; }
	const Urho3D::IntVector2 getGridSize() const override;

	short getId() override;
	float getMaxHpBarSize() const override;
	float getHealthPercent() const override;
	Urho3D::String toMultiLineString() override;
	std::string getValues(int precision) override;
	void action(ResourceActionType type, char player);
	ObjectType getType() const override { return ObjectType::RESOURCE; }
private:

	db_resource* dbResource;
	//float amount;
};
