#pragma once
#include <array>
#include <functional>
#include <span>
#include <vector>

struct db_with_cost;
class Player;
class Resources;

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
	void addRequest(WantItemType type, float priority, unsigned char count = 1, short specificId = -1);

	// Callback: returns the db_with_cost* for the item (nullptr if can't resolve).
	// If item is affordable, callback should execute it and return true.
	using ExecuteCallback = std::function<bool(WantItem& item)>;
	using CostCallback = std::function<const db_with_cost*(const WantItem& item)>;

	LackingResult execute(Player* player, const ExecuteCallback& executeFn, const CostCallback& costFn);

	const std::vector<WantItem>& getItems() const { return items; }
	int getItemCount() const { return static_cast<int>(items.size()); }

private:
	void boostOrDecay();
	void dropDead();
	void sortByPriority();
	void addLacking(const db_with_cost* cost, Resources* resources, LackingResult& lacking);

	std::vector<WantItem> items;
};
