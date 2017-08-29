#pragma once
#include <Urho3D/Container/Str.h>
#include "db_strcut.h"

class Resources
{
public:
	Resources();
	Resources(float valueForAll);
	~Resources();
	bool reduce(std::vector<db_cost*>* costs);
	void add(int id, float value);
	bool hasChanged();
	short getSize();
	float *getValues();
	Urho3D::String** getNames();
	void hasBeedUpdatedDrawn();
private:
	bool changed;
	void revert(int end, std::vector<db_cost*>* costs);
	Urho3D::String* names[RESOURCE_NUMBER_DB];
	float values[RESOURCE_NUMBER_DB];
	int size;
};
