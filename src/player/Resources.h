#pragma once

#include <string>
#include <span>
#include <vector>

#include "utils/defines.h"

struct db_with_cost;
struct db_cost;

class Resources {
public:
	Resources();
	~Resources() = default;
	void init(float valueForAll);
	explicit Resources(float valueForAll);
	Resources(const Resources&) = delete;

	bool reduce(const db_cost* costs);
	bool hasEnough(const db_cost* costs) const;
	void add(int id, float value);
	bool hasChanged() const { return changed; }

	std::span<float> getValues() const { return values; }
	std::span<float> getGatherSpeeds() const { return gatherSpeeds; }
	std::span<float> getSumValues() const { return sumValues; }

	void hasBeenUpdatedDrawn();
	int getSum() const;

	std::string getValues(int precision, int player) const;
	void setValue(int id, float amount);
	void change();

	void resetStats() const;
private:
	float data[RESOURCES_SIZE * RESOURCES_SIZE];

	std::span<float> values;
	std::span<float> gatherSpeeds;
	std::span<float> sumGatherSpeed;
	std::span<float> sumValues;

	bool changed;
};
