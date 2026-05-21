#pragma once
#include <array>
#include <span>

class Brain;
class Player;
struct db_nation;

enum class MasterOutputIdx : unsigned char {
	WORKER_URGENCY,
	ECONOMY_URGENCY,
	BUILDING_URGENCY,
	UNIT_URGENCY,
	MILITARY_URGENCY,
	EXPAND_URGENCY,
	TECH_URGENCY,
	DEFENCE_BUILDING_URGENCY,
	ATTACK_URGENCY,
};

struct MasterOutput {
	float workerUrgency;
	float economyUrgency;
	float buildingUrgency;
	float unitUrgency;
	float militaryUrgency;
	float expandUrgency;
	float techUrgency;
	float defenceBuildingUrgency;
	float attackUrgency;
};

class MasterBrain {
public:
	explicit MasterBrain(db_nation* nation);
	MasterBrain(const MasterBrain&) = delete;

	MasterOutput decide(Player* player, Player* enemy, std::span<const float> lackingPerResource, float totalLacking);

private:
	void updateHistory(Player* player);

	Brain* brain;

	// History for deltas
	float prevScore = 0.f;
	float prevUnits = 0.f;
	float prevResSum = 0.f;
};
