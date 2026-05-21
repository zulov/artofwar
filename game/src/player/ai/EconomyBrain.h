#pragma once
#include <array>
#include <span>
#include <magic_enum.hpp>

class Brain;
class Player;
struct db_nation;

enum class EconomyInputIdx : unsigned char {
	PLAYER_SCORE,
	ENEMY_SCORE,
	RES_FOOD,
	RES_WOOD,
	RES_STONE,
	RES_GOLD,
	GATHER_FOOD,
	GATHER_WOOD,
	GATHER_STONE,
	GATHER_GOLD,
	FREE_WORKERS,
	WORKERS_COUNT,
	FOOD_STORAGE_RATIO,
	GOLD_STORAGE_RATIO,
	STONE_REFINE_RATIO,
	GOLD_REFINE_RATIO,
	RES_WO_BONUS,
	LACKING_FOOD,
	LACKING_WOOD,
	LACKING_STONE,
	LACKING_GOLD,
	ECONOMY_URGENCY,
	WORKER_URGENCY,
	EXPAND_URGENCY,
};

enum class EconomyOutputIdx : unsigned char {
	WORKER_ALLOCATION,
	FOOD_PRIORITY,
	WOOD_PRIORITY,
	STONE_PRIORITY,
	GOLD_PRIORITY,
	EXPAND_PRIORITY,
};

struct EconomyOutput {
	float workerAllocation;
	float foodPriority;
	float woodPriority;
	float stonePriority;
	float goldPriority;
	float expandPriority;
};

class EconomyBrain {
public:
	explicit EconomyBrain(db_nation* nation);
	EconomyBrain(const EconomyBrain&) = delete;

	EconomyOutput decide(Player* player, Player* enemy,
	                      std::span<const float> lackingPerResource,
	                      float economyUrgency, float workerUrgency, float expandUrgency);

private:
	Brain* brain;
	std::array<float, magic_enum::enum_count<EconomyInputIdx>()> inputData{};
};
