#include "WantList.h"

#include <algorithm>
#include <ranges>
#include "database/db_struct.h"
#include "player/Player.h"
#include "player/Resources.h"

void WantList::beginTick() {
	for (auto& item : items) {
		item.active = false;
	}
}

void WantList::addRequest(WantItemType type, float priority, unsigned char count, short specificId) {
	for (auto& item : items) {
		if (item.type == type && item.specificId == specificId) {
			item.active = true;
			item.basePriority = priority;
			item.count = count;
			return;
		}
	}
	items.push_back(WantItem(priority, type, count, specificId));
}

void WantList::boostOrDecay() {
	for (auto& item : items) {
		if (item.active) {
			item.priority = item.basePriority * (1.f + BOOST_FACTOR * item.age);
			item.age++;
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
	for (int i = 0; i < 4; ++i) {
		float deficit = static_cast<float>(cost->values[i]) - resources->getValues()[i];
		if (deficit > 0.f) {
			lacking.perResource[i] += deficit;
			lacking.totalSum += deficit;
		}
	}
}

WantList::LackingResult WantList::execute(Player* player, const ExecuteCallback& executeFn, const CostCallback& costFn) {
	LackingResult lacking{};
	lacking.perResource.fill(0.f);
	lacking.totalSum = 0.f;

	boostOrDecay();
	sortByPriority();

	auto* resources = player->getResources();

	for (auto& item : items) {
		const auto* cost = costFn(item);

		for (unsigned char i = 0; i < item.count; ++i) {
			if (!cost || !resources->hasEnough(cost)) {
				addLacking(cost, resources, lacking);
				break;
			}
			if (!executeFn(item)) {
				break;
			}
		}
	}

	dropDead();
	return lacking;
}
