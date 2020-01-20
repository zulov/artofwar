#pragma once
#include "database/db_strcut.h"
#include <string>

class Resources
{
public:
	Resources();
	void init(float valueForAll);
	explicit Resources(float valueForAll);
	~Resources();
	bool reduce(std::vector<db_cost*>* costs);
	void add(int id, float value);
	bool hasChanged() const;
	short getSize() const;
	float *getValues();
	void hasBeedUpdatedDrawn();
	std::string getValues(int precision, int player);
	void setValue(int id, float amount);
	void change();
	static std::string getColumns();
private:
	void revert(int end, std::vector<db_cost*>* costs);
	float values[RESOURCE_NUMBER_DB];
	int size;
	bool changed;
};
