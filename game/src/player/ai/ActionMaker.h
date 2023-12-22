#pragma once
#include <optional>
#include <span>
#include <valarray>
#include <vector>


namespace Urho3D {
	class Vector2;
}

enum class ParentBuildingType : char;
enum class AiActionType : char;

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
class Brain;

class ActionMaker {
public:
	explicit ActionMaker(Player* player, db_nation* nation);
	ActionMaker(const ActionMaker& rhs) = delete;
	void action();
private:
	bool createUnit(db_unit* unit, Building* building) const;
	bool createUnit(db_unit* unit) const;
	bool createWorker(db_unit* unit) const;

	bool enoughResources(const db_with_cost* withCosts, Player * player) const;

	bool levelUpUnit();
	bool levelUpBuilding();
	bool createBuilding(db_building* building, ParentBuildingType type) const;
	std::vector<db_building*> getBuildingsInType(ParentBuildingType type);
	bool createBuilding(std::span<float> buildingsInput);
	bool createUnit(std::span<float> unitsInput);
	bool createWorker() const;

	std::span<float> getWhichBuilding(ParentBuildingType type, const std::span<float> aiTypeInput) const;

	std::optional<Urho3D::Vector2> findPosToBuild(db_building* building, ParentBuildingType type) const;
	std::vector<Building*> getBuildingsCanDeploy(short unitId) const;

	float dist(std::valarray<float>& center, const db_basic_metric* metric);
	float dist(std::valarray<float>& center, const db_building_metric* metric, ParentBuildingType type);

	db_building* chooseBuilding(std::span<float> result, ParentBuildingType type);
	db_building_level* chooseBuildingLevelUp();
	db_unit* chooseUnit(std::span<float> result);
	db_unit_level* chooseUnitLevelUp();

	Building* getBuildingToDeploy(db_unit* unit) const;
	Building* getBuildingToDeployWorker(db_unit* unit) const;
	Building* getBuildingToLevelUpUnit(db_unit_level* level);
	Building* getBuildingClosestArea(std::vector<Building*>& allPossible, std::span<float> result) const;

	bool isEnoughResToWorker() const;
	bool isEnoughResToAnyUnit() const;
	bool isEnoughResToAnyBuilding() const;
	bool isEnoughResToTypeBuilding(ParentBuildingType type) const;

	Player* player;
	db_nation* nation;

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

