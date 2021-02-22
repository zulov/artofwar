#pragma once

#include <string>
#include <span>
#include <vector>

struct db_cost;

class Resources {
public:
	Resources();
	~Resources() = default;
	void init(float valueForAll);
	explicit Resources(float valueForAll);
	Resources(const Resources&) = delete;

	bool reduce(const std::vector<db_cost*>& costs);
	bool hasEnough(const std::vector<db_cost*>& costs) const;
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
	float data[4 * 4];

	std::span<float> values;
	std::span<float> gatherSpeeds;
	std::span<float> sumGatherSpeed;
	std::span<float> sumValues;

	int size;
	bool changed;
};
