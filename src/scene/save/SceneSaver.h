#pragma once
#include <Urho3D/Container/Str.h>
#include "database/DatabaseCache.h"
#include "objects/unit/Unit.h"
#include "Loading.h"
#include "objects/building/Building.h"

class SceneSaver
{
public:
	SceneSaver(int _precision);
	~SceneSaver();
	void createUnitsTable();
	void createBuildingsTable();
	void createTables();
	void createDatabase(Urho3D::String fileName);
	void createSave(Urho3D::String fileName);
	void saveUnits(std::vector<Unit*>* units);
	void saveBuildings(std::vector<Building*>* buildings);
	void saveResources(std::vector<ResourceEntity*>* resources);
	void close();
private:
	sqlite3* database;
	int precision;
	int rc;
	loading * loadingState;
};

