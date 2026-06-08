#include "WantList.h"

#include <algorithm>
#include <ranges>

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
