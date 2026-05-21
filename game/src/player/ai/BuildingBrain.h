#pragma once
#include <span>

class Brain;
class Player;
struct db_nation;

enum class BuildingOutputIdx : unsigned char {
	OTHER_URGENCY,
	DEFENCE_URGENCY,
	RESOURCE_URGENCY,
	TECH_URGENCY,
	UNITS_URGENCY,
	FOOD_RES_PREF,
	WOOD_RES_PREF,
	STONE_RES_PREF,
	GOLD_RES_PREF,
	BLACKSMITH_PREF,
	UNIVERSITY_PREF,
};

struct BuildingOutput {
	float otherUrgency;
	float defenceUrgency;
	float resourceUrgency;
	float techUrgency;
	float unitsUrgency;
	float foodResPref;
	float woodResPref;
	float stoneResPref;
	float goldResPref;
	float blacksmithPref;
	float universityPref;
};

class BuildingBrain {
public:
	explicit BuildingBrain(db_nation* nation);
	BuildingBrain(const BuildingBrain&) = delete;

	BuildingOutput decide(Player* player, Player* enemy,
	                       float buildingUrgency, float techUrgency, float defenceBuildingUrgency);

private:
	Brain* brain;
};
