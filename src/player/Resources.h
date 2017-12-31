#pragma once
#include <Urho3D/Container/Str.h>
#include "database/db_strcut.h"

class Resources
{
public:
	Resources();
	void init(double valueForAll);
	Resources(double valueForAll);
	~Resources();
	bool reduce(std::vector<db_cost*>* costs);
	void add(int id, double value);
	bool hasChanged();
	short getSize();
	double *getValues();
	void hasBeedUpdatedDrawn();
	std::string getValues(int precision, int player);
	void setValue(int id, float amount);
	static std::string getColumns();
private:
	bool changed;
	void revert(int end, std::vector<db_cost*>* costs);
	double values[RESOURCE_NUMBER_DB];
	int size;
};
