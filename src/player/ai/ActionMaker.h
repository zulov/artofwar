#pragma once
#include <optional>
#include <span>
#include <valarray>
#include <vector>


struct db_nation;
enum class AiActionType : char;

namespace Urho3D {
	class Vector2;
}

struct db_basic_metric;
class Player;
class Building;
struct db_with_cost;
struct db_building;
struct db_unit_level;
struct db_unit;
struct db_building_level;
struct Brain;

class ActionMaker {
public:
	explicit ActionMaker(Player* player, db_nation* nation);
	ActionMaker(const ActionMaker& rhs) = delete;
	void action();
private:
	bool createUnit(db_unit* unit, Building* building) const;
	bool createUnit(db_unit* unit) const;
	bool createWorker(db_unit* unit) const;

	bool enoughResources(db_with_cost* withCosts) const;

	bool levelUpUnit();
	bool levelUpBuilding();
	bool createBuilding(db_building* building);
	bool createBuilding(std::span<float> buildingsInput);
	bool createUnit(std::span<float> unitsInput);
	bool createWorker();

	bool execute(const std::span<float> unitsInput, const std::span<float> buildingsInput, AiActionType decision);

	std::optional<Urho3D::Vector2> findPosToBuild(db_building* building) const;
	std::vector<Building*> getBuildingsCanDeploy(short unitId) const;

	float dist(std::valarray<float>& center, const db_basic_metric* metric);

	db_building* chooseBuilding(std::span<float> result);
	db_building_level* chooseBuildingLevelUp();
	db_unit* chooseUnit(std::span<float> result);
	db_unit_level* chooseUnitLevelUp();

	Building* getBuildingToDeploy(db_unit* unit) const;
	Building* getBuildingToDeployWorker(db_unit* unit) const;
	Building* getBuildingToLevelUpUnit(db_unit_level* level);
	Building* getBuildingClosestArea(std::vector<Building*>& allPossible, std::span<float> result) const;

	Player* player;

	Brain* ifWorkerCreate;
	Brain* whereWorkerCreate;

	Brain* ifBuildingCreate;
	Brain* whichBuildingCreate;
	Brain* whereBuildingCreate;

	Brain* ifUnitCreate;
	Brain* whichUnitCreate;
	Brain* whereUnitCreate;

};
