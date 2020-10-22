#pragma once
#include "objects/static/Static.h"

namespace Urho3D {
	class Vector3;
}

struct dbload_resource_entities;
struct db_resource;
enum class ResourceActionType : char;

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
	Urho3D::String toMultiLineString() override;
	std::string getValues(int precision) override;

	unsigned short getMaxHp() const override;
	void action(ResourceActionType type, char player);
	ObjectType getType() const override { return ObjectType::RESOURCE; }
private:

	db_resource* dbResource;
};
