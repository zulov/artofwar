#include "WantList.h"

#include <algorithm>
#include <ranges>
#include "database/db_struct.h"
#include "player/Player.h"
#include "player/Resources.h"

void WantList::resetRequests() {
	for (auto& item : items) {
		item.active = false;
	}
}

void WantList::addRequest(WantItemType type, float priority, short specificId, unsigned char count) {
	for (auto& item : items) {
		if (item.type == type && item.specificId == specificId) {
			item.active = true;
			item.basePriority = priority;
			item.count = count;
			return;
		}
	}
	items.emplace_back(priority, type, count, specificId);
}

void WantList::boostOrDecay() {
	for (auto& item : items) {
		if (item.active) {
			const float age = static_cast<float>(item.age);
			const float boost = BOOST_MAX * age / (age + BOOST_HALF_AGE);
			item.priority = item.basePriority * (1.f + boost);
			if (item.age < 1000) { item.age++; }
		} else {
			item.priority *= DECAY_FACTOR;
		}
	}
}

void WantList::dropDead() {
	std::erase_if(items, [](const WantItem& item) {
		return !item.active && item.priority < DROP_THRESHOLD;
	});
}

void WantList::sortByPriority() {
	std::ranges::sort(items, [](const WantItem& a, const WantItem& b) {
		return a.priority > b.priority;
	});
	if (static_cast<int>(items.size()) > MAX_ITEMS) {
		items.resize(MAX_ITEMS);
	}
}

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
