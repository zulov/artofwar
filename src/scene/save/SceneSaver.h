#pragma once

#include "Loading.h"
#include <sqlite3/sqlite3.h>
#include <vector>


namespace Urho3D {
	class String;
}

class Unit;
class ResourceEntity;
class Building;
class Player;

class SceneSaver {
public:
	explicit SceneSaver(int _precision);
	~SceneSaver();

	void createTables();
	void createDatabase(const Urho3D::String& fileName);
	void createSave(const Urho3D::String& fileName);
	void saveUnits(std::vector<Unit*>* units);
	void saveBuildings(std::vector<Building*>* buildings);
	void saveResourceEntities(std::vector<ResourceEntity*>* resources);
	void savePlayers(std::vector<Player*>& players);
	void saveResources(const std::vector<Player*>& players);
	void saveConfig();
	void close();
private:
	void executeInsert(std::string& sqlstatement);

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
