#pragma once
#include <array>
#include <span>

class Brain;
class Player;
struct db_nation;

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
	std::array<float, 20> inputData{};
};
