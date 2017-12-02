#pragma once
#include <Urho3D/Container/Str.h>
#include "database/DatabaseCache.h"
#include "objects/unit/Unit.h"
#include "Loading.h"

class SceneSaver
{
public:
	SceneSaver(int _precision);
	~SceneSaver();
	void createUnitsTable();
	void createDatabase(Urho3D::String fileName);
	void createSave(Urho3D::String fileName);
	void saveUnits(std::vector<Unit*>* units);
private:
	sqlite3* database;
	int precision;
	int rc;
	loading * loadingState;
};

