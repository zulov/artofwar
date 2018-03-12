#pragma once

#include "objects/unit/Unit.h"
#include "objects/building/Building.h"
#include "player/Player.h"
#include <sqlite3/sqlite3.h>
#include "Loading.h"

struct loading;

class SceneSaver
{
public:
	SceneSaver(int _precision);
	~SceneSaver();
	
	void createTables();
	void createDatabase(const Urho3D::String& fileName);
	void createSave(Urho3D::String fileName);
	void saveUnits(std::vector<Unit*>* units);
	void saveBuildings(std::vector<Building*>* buildings);
	void saveResourceEntities(std::vector<ResourceEntity*>* resources);
	void savePlayers(std::vector<Player*>& players);
	void saveResources(const std::vector<Player*>& players);
	void saveConfig();
	void close();
private:
	void executeInsert(std::string sqlstatement);

	void createUnitsTable();
	void createTable(const std::string& sql);
	void createBuildingsTable();
	void createResourceEntitiesTable();
	void createPlayerTable();
	void createConfigTable();
	void createResourceTable();

	sqlite3* database;
	int precision;

	loading loadingState;
};

