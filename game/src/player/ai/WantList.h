#pragma once
#include <array>
#include <span>
#include <vector>

struct db_with_cost;
class Player;
class Resources;
struct WantItem;

// Strategy for resolving a want's cost and carrying it out.
// Keeps WantList a pure scheduler: it never needs to know how items are built.
class IWantExecutor {
public:
	virtual ~IWantExecutor() = default;
	// Returns the cost of the item (nullptr if it can't be resolved).
	virtual const db_with_cost* cost(const WantItem& item) const = 0;
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
	bool active{};

	WantItem() = default;

	WantItem(float priority, float basePriority, WantItemType type, unsigned char count, short specificId, unsigned short age, bool active) :
		priority(priority), basePriority(basePriority),
		type(type),
		count(count),
		specificId(specificId), age(age), active(active) {}

	WantItem(float priority, WantItemType type, unsigned char count, short specificId) :
		priority(priority), basePriority(priority),
		type(type),
		count(count),
		specificId(specificId), age(0), active(true) {}
};

class WantList {
public:
	static constexpr int MAX_ITEMS = 32;
	static constexpr float BOOST_FACTOR = 1.15f;
	static constexpr float DECAY_FACTOR = 0.85f;
	static constexpr float DROP_THRESHOLD = 0.05f;

	struct LackingResult {
		std::array<float, 4> perResource;
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
	LackingResult execute(Player* player, IWantExecutor& executor);

	const std::vector<WantItem>& getItems() const { return items; }
	int getItemCount() const { return static_cast<int>(items.size()); }

private:
	void boostOrDecay();
	void dropDead();
	void sortByPriority();
	void addLacking(const db_with_cost* cost, Resources* resources, LackingResult& lacking);

	std::vector<WantItem> items;
};
