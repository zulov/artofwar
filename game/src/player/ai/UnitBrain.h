#pragma once
#include <array>
#include <span>

class Brain;
class Player;
struct db_nation;

enum class UnitOutputIdx : unsigned char {
	UNIT_PROFILE,  // indices 0..23 (24-dim profile vector)
	COUNT = 24,    // derived from unitUrgency, not a NN output
};

struct UnitOutput {
	std::array<float, 24> unitProfile;  // ideal unit property vector
	unsigned char count;                 // how many to produce
};

class UnitBrain {
public:
	static constexpr int MAX_UNITS_PER_TICK = 5;

	explicit UnitBrain(db_nation* nation);
	UnitBrain(const UnitBrain&) = delete;

	UnitOutput decide(Player* player, Player* enemy,
	                   float unitUrgency, float attackUrgency);

private:
	Brain* brain;
	std::array<float, 14> inputData{};
};
