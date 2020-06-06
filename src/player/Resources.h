#pragma once
#include "database/db_strcut.h"
#include <string>

class Resources {
public:
	Resources();
	void init(float valueForAll);
	explicit Resources(float valueForAll);
	~Resources();
	bool reduce(const std::vector<db_cost*>& costs);
	bool hasEnough(const std::vector<db_cost*>& costs) const;
	void add(int id, float value);
	bool hasChanged() const;
	short getSize() const;
	float* getValues();
	float* getGatherSpeeds();
	void hasBeenUpdatedDrawn();
	std::string getValues(int precision, int player);
	void setValue(int id, float amount);
	void change();
	static std::string getColumns();
	void resetStats() const;
private:
	void revert(int end, const std::vector<db_cost*>& costs);

	float* values;
	float* gatherSpeeds;
	float* sumGatherSpeed;
	int size;
	bool changed;
};

