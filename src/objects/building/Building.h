#pragma once
#include "objects/static/Static.h"
#include "objects/queue/QueueManager.h"

enum class BuildingActionType : char;
struct db_building;
struct db_building_level;
struct db_unit;
class QueueElement;

class Building : public Static {
public:
	Building(Urho3D::Vector3& _position, int id, int player, int level, int mainCell);
	~Building();

	void populate() override;
	void levelUp(char level);
	Building* load(dbload_building* dbloadBuilding);
	static std::string getColumns();

	QueueElement* updateQueue(float time) const { return queue->update(time); }

	std::optional<int> getDeploy() override {
		if (deployIndex > -1) {
			return deployIndex;
		}
		return {};
	}

	//TODO target to nie to samo co gdzie sie maja pojawiac!
	QueueManager* getQueue() const { return queue; }

	Urho3D::String toMultiLineString() override;
	float absorbAttack(float attackCoef) override;
	ObjectType getType() const override { return ObjectType::BUILDING; }
	void action(BuildingActionType type, short id) const;
	std::string getValues(int precision) override;
	float getMaxHpBarSize() const override;
	short getId() override;
	int getLevel() override { return dbLevel->level; }
	void fillValues(std::span<float> weights) const;
	void addValues(std::span<float> vals) const override;
	const Urho3D::IntVector2 getGridSize() const override;
	unsigned short getMaxHp() const override;
	void createDeploy();
private:

	int deployIndex;
	db_building* dbBuilding;
	db_building_level* dbLevel;

	QueueManager* queue;
};
