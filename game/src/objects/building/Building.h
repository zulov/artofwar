#pragma once
#include "objects/queue/QueueManager.h"
#include "objects/static/Static.h"
#include "database/db_insert_utils.h"
#include "database/db_struct.h"

struct ProjectileWithNode;
class AbstractQueueManager;
struct dbload_building;
enum class BuildingActionType : char;
class QueueElement;

class Building : public Static {
	friend void bindRow<Building>(sqlite3_stmt*, int, const Building*);

public:
	Building(const Urho3D::Vector3& _position, db_building* db_building, unsigned char playerId, unsigned char teamId,
			 unsigned char level, int indexInGrid, UId uId);
	~Building() override;

	void populate();
	void levelUp(char level);
	void postCreate();
	Building* load(dbload_building* dbloadBuilding);

	QueueElement* updateQueue();
    void updateAi(bool ifBuildingAction);

    bool isDeadOrTooFar() const;

    std::optional<int> getDeploy();

	//TODO target to nie to samo co gdzie sie maja pojawiac!
	const QueueManager& getQueue() const { return queue; }
	bool isReady() const { return state != StaticState::CREATING; }

	Urho3D::String getInfo() const override;
	const Urho3D::String& getName() const override;

	std::pair<float, bool> absorbAttack(float attackCoef) override;

	ObjectType getType() const override { return ObjectType::BUILDING; }
	float getAttackVal(Physical* aim) override;
	void action(BuildingActionType type, unsigned short id);
	float getMaxHpBarSize() const override;
	float getHealthBarSize() const override;
	char getLevelNum() const override;
	const Urho3D::UCharVector2 getGridSize() const override;
	void createDeploy();
	void setDeploy(int cell);
	void complete();

	float getSightRadius() const override;
	short getCostSum() const override;

	bool canUse(int index) const override;
	db_building_level* getLevel() const;

	float getModelHeight() const override;
	void setModelData(float modelHeight) const override;
	db_building* getDb() const { return getDbBuilding(); }
	std::pair<db_building*, db_building_level*> getData() const { return {getDbBuilding(), dbLevel}; }
	unsigned char getMaxCloseUsers() const override;
	//TODO getUniT DATA i resource
private:
	db_building* getDbBuilding() const { return static_cast<db_building*>(dbEntity); }
	int deployIndex = -1;
	unsigned short currentFrameState = 0;

	db_building_level* dbLevel;

	QueueManager queue;
	Physical* thingToInteract{};
};
