#pragma once

#include <string>
#include <span>

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

	std::span<float> getValues() const { return values; }
	std::span<float> getGatherSpeeds() const { return gatherSpeeds; }
	std::span<float> getSumValues() const { return sumValues; }
	std::span<float> getResWithOutBonus() const { return resWithoutBonus; }

	int getSum() const;

	std::string getValues(int precision, int player) const;
	void setValue(int id, float amount);

	void resetStats() const;
private:
	float data[RESOURCES_SIZE * 5];

	std::span<float> values;
	std::span<float> gatherSpeeds;
	std::span<float> sumGatherSpeed;
	std::span<float> sumValues;
	std::span<float> resWithoutBonus;
};
