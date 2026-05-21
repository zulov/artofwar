#pragma once
#include <array>
#include <span>

class Brain;
class Player;
struct db_nation;

enum class MilitaryOutputIdx : unsigned char {
	ATTACK_WEIGHT,
	DEFEND_WEIGHT,
	ATTACK_TARGET,
	DEFEND_TARGET,
};

struct MilitaryOutput {
	float attackWeight;
	float defendWeight;
	float attackTarget;
	float defendTarget;
};

class MilitaryBrain {
public:
	explicit MilitaryBrain(db_nation* nation);
	MilitaryBrain(const MilitaryBrain&) = delete;

	MilitaryOutput decide(Player* player, Player* enemy,
	                       float militaryUrgency, float attackUrgency);

private:
	Brain* brain;
	std::array<float, 14> inputData{};
};
