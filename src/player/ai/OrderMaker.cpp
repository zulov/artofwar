#include "OrderMaker.h"
#include "objects/unit/Unit.h"
#include "stats/StatsEnums.h"

OrderMaker::OrderMaker(Player* player)
	: player(player), collectResourceId("Data/ai/main_w.csv"){
}

void OrderMaker::action() {
	auto freeWorkers = findFreeWorkers();
	
	if (!freeWorkers.empty()) {
		collect(freeWorkers);
	}
}

bool OrderMaker::isNotInStates(UnitState state, std::initializer_list<UnitState> states) {
	for (auto s : states) {
		if (state == s) {
			return false;
		}
	}
	return true;
}

std::vector<Unit*> OrderMaker::findFreeWorkers() {
	std::vector<Unit*> freeWorkers;
	for (auto worker : player->getPossession().getWorkers()) {
		if (isNotInStates(worker->getState(), {UnitState::COLLECT, UnitState::GO_TO})) {
			freeWorkers.push_back(worker);
		}
	}
}

bool OrderMaker::collect(std::vector<Unit*>& workers) {
	auto values = player->getResources().getValues();

	float input[magic_enum::enum_count<ResourceInputType>()];
	
	auto result = collectResourceId.decide(input);

	
}
