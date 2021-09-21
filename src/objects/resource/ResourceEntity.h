#pragma once
#include "objects/static/Static.h"

namespace Urho3D
{
	class Vector3;
}

struct dbload_resource_entities;
struct db_resource;
enum class ResourceActionType : char;

class ResourceEntity : public Static {
public:
	ResourceEntity(Urho3D::Vector3 _position, int id, int level, int mainCell, bool withNode);
	virtual ~ResourceEntity() = default;
	void populate() override;

	std::pair<float, bool> absorbAttack(float collectSpeed) override;
	ResourceEntity* load(dbload_resource_entities* resource);

	char getPlayer() const override { return -1; }
	const Urho3D::IntVector2 getGridSize() const override;

	float getMaxHpBarSize() const override;
	Urho3D::String getInfo() const override;
	const Urho3D::String& getName() const override;

	std::string getValues(int precision) override;

	void action(ResourceActionType type, char player);

	bool canUse(int index) const override;

	ObjectType getType() const override { return ObjectType::RESOURCE; }
	unsigned char getMaxCloseUsers() const override;
	short getCostSum() const override { return 0; }
private:
	db_resource* dbResource;
};
