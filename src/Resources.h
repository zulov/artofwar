#pragma once
#include <Urho3D/Container/Str.h>
#include "db_strcut.h"

class Resources
{
public:
	Resources();
	Resources(float valueForAll);
	~Resources();
	
	bool reduce(float * cost);
	void add(int id, float value);

private:
	void revert(int i, float* cost);
	Urho3D::String * names[RESOURCE_NUMBER_DB];
	float values[RESOURCE_NUMBER_DB];
	int size;
};

