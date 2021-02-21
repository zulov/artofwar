#pragma once

#include <string>
#include <span>
#include <vector>

struct db_cost;

class Resources {
public:
	Resources();
	void init(float valueForAll);
	explicit Resources(float valueForAll);
	Resources(const Resources&) = delete;
	~Resources();
	bool reduce(const std::vector<db_cost*>& costs);
	bool hasEnough(const std::vector<db_cost*>& costs) const;
	void add(int id, float value);
	bool hasChanged() const { return changed; }

	std::span<float> getValues() const {return valuesSpan;}
	std::span<float> getGatherSpeeds() const { return gatherSpeedsSpan; }
	std::span<float> getSumValues() const { return sumValuesSpan; }
	
	void hasBeenUpdatedDrawn();
	int getSum() const;
	
	std::string getValues(int precision, int player) const;
	void setValue(int id, float amount);
	void change();

	void resetStats() const;
private:
	void revert(int end, const std::vector<db_cost*>& costs);

	float* values;
	float* gatherSpeeds;
	float* sumGatherSpeed;
	float* sumValues;
	
	std::span<float> valuesSpan;
	std::span<float> gatherSpeedsSpan;
	std::span<float> sumGatherSpeedSpan;
	std::span<float> sumValuesSpan;
	
	int size;
	bool changed;
};

