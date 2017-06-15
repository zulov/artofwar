#pragma once
#include <Urho3D/Container/Str.h>
#include "db_strcut.h"

class Resources
{
public:
	Resources();
	~Resources();
private:
	Urho3D::String * names[RESOURCE_NUMBER_DB];
	int values[RESOURCE_NUMBER_DB];
};

