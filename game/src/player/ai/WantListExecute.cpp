#include "WantList.h"

#include "database/db_struct.h"
#include "player/Player.h"
#include "player/Resources.h"

// The execution half of WantList. Kept in its own translation unit because it
// depends on Player/Resources (and their heavy transitive headers), whereas the
// scheduler half (WantList.cpp) is dependency-light and unit-testable on its own.

void WantList::addLacking(const db_with_cost* cost, Resources* resources, LackingResult& lacking) {
	if (!cost) { return; }
	for (int i = 0; i < RESOURCES_SIZE; ++i) {
		float deficit = static_cast<float>(cost->values[i]) - resources->getValues()[i];
		if (deficit > 0.f) {
			lacking.perResource[i] += deficit;
			lacking.totalSum += deficit;
		}
	}
}

WantList::LackingResult WantList::execute(Player* player, IWantExecutor& executor) {
	LackingResult lacking{};
	lacking.reset();

	boostOrDecay();
	sortByPriority();

	auto* resources = player->getResources();

	for (auto& item : items) {
		const auto* cost = executor.cost(item);

		for (unsigned char i = 0; i < item.count; ++i) {
			if (!cost || !resources->hasEnough(cost)) {
				addLacking(cost, resources, lacking);
				break;
			}
			if (!executor.execute(item)) {
				break;
			}
		}
	}

	dropDead();
	return lacking;
}
