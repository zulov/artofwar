#pragma once
#include <optional>
#include <span>
#include <valarray>
#include <vector>


#include "AiHistoryEnums.h"

struct DecideResult;

class AiInputProvider;
class Possession;

namespace Urho3D {
	class Vector2;
}

enum class ParentBuildingType : char;

class Player;
class Building;
struct db_building_metric;
struct db_nation;
struct db_basic_metric;
struct db_with_cost;
struct db_building;
struct db_unit_level;
struct db_unit;
struct db_building_level;
class AiHistory;
class Brain;

class ActionMaker {
public:
	explicit ActionMaker(Player* player, db_nation* nation, AiHistory* history);
	ActionMaker(const ActionMaker& rhs) = delete;
	void action();
private:
	void createUnit(AiActionType actionType, db_unit* unit, Building* building);
	void createUnit(AiActionType actionType, db_unit* unit);
	void createWorker(AiActionType actionType, db_unit* unit);

	bool enoughResources(const db_with_cost* withCosts, Player * player) const;

	bool levelUpUnit();
	bool levelUpBuilding();
	void createBuilding(AiActionType actionType, db_building* building, ParentBuildingType type);
	std::vector<db_building*> getBuildingsInType(ParentBuildingType type);
	void createBuilding(std::span<const float> buildingsInput);
	void createUnit(std::span<const float> unitsInput);
	void createWorker();

	std::optional<Urho3D::Vector2> findPosToBuild(db_building* building, ParentBuildingType type) const;
	std::vector<Building*> getBuildingsCanDeploy(short unitId) const;

	float dist(std::valarray<float>& center, const db_basic_metric* metric);
	float dist(std::valarray<float>& center, const db_building_metric* metric, ParentBuildingType type);

	db_building* chooseBuilding(ParentBuildingType type);
	Brain* getBrainForBuildingType(ParentBuildingType type) const;
	db_building_level* chooseBuildingLevelUp();
	db_unit* chooseUnit(const DecideResult& result);
	db_unit_level* chooseUnitLevelUp();

	Building* getBuildingToDeploy(db_unit* unit) const;
	Building* getBuildingToDeployWorker(db_unit* unit) const;
	Building* getBuildingToLevelUpUnit(db_unit_level* level);
	Building* getBuildingClosestArea(std::vector<Building*>& allPossible, const DecideResult& result) const;

	bool isEnoughResToWorker() const;
	bool isEnoughResToAnyUnit() const;
	bool isEnoughResToAnyBuilding() const;
	bool isEnoughResToTypeBuilding(ParentBuildingType type) const;

	Player* player;
	unsigned char playerId;
	Possession* possession;
	db_nation* nation;
	AiInputProvider* aiInput;
	AiHistory* history;

	Brain* ifWorker;
	Brain* whereWorker;

	Brain* ifBuilding;
	Brain* whichBuildingType;
	Brain* whichBuildingTypeOther;
	Brain* whichBuildingTypeDefence;
	Brain* whichBuildingTypeResource;
	Brain* whichBuildingTypeTech;
	Brain* whichBuildingTypeUnits;
	Brain* whereBuilding;
	Brain* ifFarmBuild;

	Brain* ifUnit;
	Brain* whichUnit;
	Brain* whereUnit;

};

