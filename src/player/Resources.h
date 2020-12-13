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
	bool hasChanged() const;

	std::span<float> getValues() const;
	std::span<float> getGatherSpeeds() const;
	
	void hasBeenUpdatedDrawn();
	std::string getValues(int precision, int player);
	void setValue(int id, float amount);
	void change();

	void resetStats() const;
private:
	void revert(int end, const std::vector<db_cost*>& costs);

	float* values;
	float* gatherSpeeds;
	float* sumGatherSpeed;
	
	std::span<float> valuesSpan;
	std::span<float> gatherSpeedsSpan;
	std::span<float> sumGatherSpeedSpan;
	
	int size;
	bool changed;
};

