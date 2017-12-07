#pragma once
#include <Urho3D/Container/Str.h>
#include "database/DatabaseCache.h"
#include "objects/unit/Unit.h"
#include "Loading.h"
#include "objects/building/Building.h"
#include "player/Player.h"

class SceneSaver
{
public:
	SceneSaver(int _precision);
	~SceneSaver();
	void createUnitsTable();
	void createTable(string sql);
	void createBuildingsTable();
	void createResourceEntitiesTable();
	void createPlayerTable();
	void createConfigTable();
	void createResourceTable();
	void createTables();
	void createDatabase(Urho3D::String fileName);
	void createSave(Urho3D::String fileName);
	void executeInsert(string sqlstatement);
	void saveUnits(std::vector<Unit*>* units);
	void saveBuildings(std::vector<Building*>* buildings);
	void saveResourceEntities(std::vector<ResourceEntity*>* resources);
	void savePlayers(std::vector<Player*>& players);
	void saveResources(const vector<Player*>& players);
	void saveConfig();
	void close();
private:
	sqlite3* database;
	int precision;
	int rc;
	loading * loadingState;
};

