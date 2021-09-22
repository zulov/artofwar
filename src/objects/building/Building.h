#pragma once
#include "objects/static/Static.h"


struct MissileData;
class AbstractQueueManager;
struct dbload_building;
enum class BuildingActionType : char;
struct db_building;
struct db_building_level;
struct db_unit;
class QueueElement;

class Building : public Static {
public:
	Building(Urho3D::Vector3 _position, int id, int player, int level, int mainCell, bool withNode);
	~Building();

	void populate() override;
	void levelUp(char level);
	void postCreate();
	Building* load(dbload_building* dbloadBuilding);

	QueueElement* updateQueue(float time) const;

	std::optional<int> getDeploy();

	//TODO target to nie to samo co gdzie sie maja pojawiac!
	AbstractQueueManager* getQueue() const { return queue; }
	bool isReady() const { return ready; }

	Urho3D::String getInfo() const override;
	const Urho3D::String& getName() const override;

	std::pair<float, bool> absorbAttack(float attackCoef) override;

	ObjectType getType() const override { return ObjectType::BUILDING; }
	void action(BuildingActionType type, short id) const;
	std::string getValues(int precision) override;
	float getMaxHpBarSize() const override;
	float getHealthBarSize() const override;
	char getLevelNum() override;
	void fillValues(std::span<float> weights) const;
	void addValues(std::span<float> vals) const override;
	const Urho3D::IntVector2 getGridSize() const override;
	void createDeploy();
	void setDeploy(int cell);
	void complete();

	float getSightRadius() const override;
	short getCostSum() const override;

	bool canUse(int index) const override;
private:
	int deployIndex = -1;
	bool ready = true;
	db_building* dbBuilding;
	db_building_level* dbLevel;

	AbstractQueueManager* queue{};
	MissileData* missileData{};
};
