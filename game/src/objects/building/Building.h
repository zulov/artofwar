#pragma once
#include "objects/static/Static.h"


struct ProjectileWithNode;
class AbstractQueueManager;
struct dbload_building;
enum class BuildingActionType : char;
struct db_building;
struct db_building_level;
struct db_unit;
class QueueElement;

class Building : public Static {
public:
	Building(Urho3D::Vector3 _position, db_building* db_building, char playerId, char teamId, int level, int indexInGrid, UId uId);
	~Building() override;

	void populate();
	void levelUp(char level);
	void postCreate();
	Building* load(dbload_building* dbloadBuilding);

	QueueElement* updateQueue();
	void updateAi(bool ifBuildingAction);

	std::optional<int> getDeploy();

	//TODO target to nie to samo co gdzie sie maja pojawiac!
	AbstractQueueManager* getQueue() const { return queue; }
	bool isReady() const { return state != StaticState::CREATING; }

	Urho3D::String getInfo() const override;
	const Urho3D::String& getName() const override;

	std::pair<float, bool> absorbAttack(float attackCoef) override;

	ObjectType getType() const override { return ObjectType::BUILDING; }
	float getAttackVal(Physical* aim) override;
	void action(BuildingActionType type, short id) const;
	std::string getValues(int precision) override;
	unsigned short getMaxHpBarSize() const override;
	float getHealthBarSize() const override;
	char getLevelNum() override;
	const Urho3D::IntVector2 getGridSize() const override;
	void createDeploy();
	void setDeploy(int cell);
	void complete();

	float getSightRadius() const override;
	short getCostSum() const override;

	bool canUse(int index) const override;
	db_building_level* getLevel() const;

	float getModelHeight() const override;
	void setModelData(float modelHeight) const override;
	db_building* getDb() const { return dbBuilding; }
	std::pair<db_building*, db_building_level*> getData() const { return {dbBuilding, dbLevel}; }
	unsigned char getMaxCloseUsers() const override;
	//TODO getUniT DATA i resource
private:
	int deployIndex = -1;
	unsigned short currentFrameState = 0;

	db_building* dbBuilding;
	db_building_level* dbLevel;

	AbstractQueueManager* queue{};
	Physical* thingToInteract{};
};
