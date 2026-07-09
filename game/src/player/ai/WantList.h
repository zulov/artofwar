#pragma once
#include <array>
#include <span>
#include <vector>

#include "objects/resource/ResourceType.h"

struct db_with_cost;
struct WantItem;

// Strategy for resolving a want's cost and carrying it out.
// Keeps WantList a pure scheduler: it never needs to know how items are built.
class IWantExecutor {
public:
	virtual ~IWantExecutor() = default;
	// Returns the cost of the item (nullptr if it can't be resolved).
	virtual const db_with_cost* cost(const WantItem& item) const = 0;
	// Called when the item is otherwise valid but unaffordable.
	virtual void onNotEnoughResources(const WantItem& item) = 0;
	// Executes the item once it is known affordable; returns false to stop repeating it.
	virtual bool execute(WantItem& item) = 0;
};

enum class WantItemType : unsigned char {
	WORKER,
	UNIT,
	BUILDING,
	UNIT_UPGRADE,
	BUILDING_UPGRADE,
};

struct WantItem {
	float priority{};
	float basePriority{};
	WantItemType type{};
	unsigned char count{};
	short specificId{};
	unsigned short age{};
	unsigned char reserveTicks{};
	bool active{};

	WantItem() = default;

	WantItem(float priority, float basePriority, WantItemType type, unsigned char count, short specificId,
	         unsigned short age, unsigned char reserveTicks, bool active) :
		priority(priority), basePriority(basePriority),
		type(type),
		count(count),
		specificId(specificId), age(age), reserveTicks(reserveTicks), active(active) {}

	WantItem(float priority, WantItemType type, unsigned char count, short specificId) :
		priority(priority), basePriority(priority),
		type(type),
		count(count),
		specificId(specificId), age(0), reserveTicks(0), active(true) {}
};

class WantList {
public:
	static constexpr int MAX_ITEMS = 32;
	// Persistence boost: a continuously-requested want ramps up with age but with
	// diminishing returns toward a hard ceiling, so it can never starve the queue.
	//   multiplier = 1 + BOOST_MAX * age / (age + BOOST_HALF_AGE)
	// age 0 -> x1, age == BOOST_HALF_AGE -> x(1 + BOOST_MAX/2), age -> inf -> x(1 + BOOST_MAX)
	static constexpr float BOOST_MAX = 2.0f;       // ceiling: up to +200% (x3 base) as age grows
	static constexpr float BOOST_HALF_AGE = 8.0f;  // age at which half of BOOST_MAX is reached
	static constexpr float DECAY_FACTOR = 0.85f;
	static constexpr float DROP_THRESHOLD = 0.05f;

	static constexpr unsigned char SOFT_RESERVE_STEP_PERCENT = 1;
	static constexpr unsigned char SOFT_RESERVE_MAX_PERCENT = 33;

	struct LackingResult {
		std::array<float, RESOURCES_SIZE> perResource;
		float totalSum;
		short lackingBuildingForUnit = -1;

		void reset() {
			perResource.fill(0.f);
			totalSum = 0.f;
			lackingBuildingForUnit = -1;
		}
	};

	void resetRequests();
	void addRequest(WantItemType type, float priority, short specificId = -1, unsigned char count = 1);

	// Callback: returns the db_with_cost* for the item (nullptr if can't resolve).
	// If item is affordable, callback should execute it and return true.
	LackingResult execute(std::span<const float> resourceValues, IWantExecutor& executor);

	const std::vector<WantItem>& getItems() const { return items; }
	int getItemCount() const { return static_cast<int>(items.size()); }

private:
	void boostOrDecay();
	void dropDead();
	void sortByPriority();
	void addLacking(const db_with_cost* cost, std::span<const float> resourceValues, LackingResult& lacking);

	std::vector<WantItem> items;
};
