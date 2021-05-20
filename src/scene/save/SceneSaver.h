#pragma once

#include <vector>
#include "Loading.h"

namespace Urho3D {
	class String;
}

class sqlite3;
class Unit;
class ResourceEntity;
class Building;
class Player;

class SceneSaver {
public:
	explicit SceneSaver(int precision);
	~SceneSaver() = default;

	void createTables() const;
	void createDatabase(const Urho3D::String& fileName);
	void createSave(const Urho3D::String& fileName);
	void saveUnits(std::vector<Unit*>* units);
	void saveBuildings(std::vector<Building*>* buildings);
	void saveResourceEntities(std::vector<ResourceEntity*>* resources);
	void savePlayers(std::vector<Player*>& players);
	void saveResources(const std::vector<Player*>& players);
	void saveConfig(int mapId, int size);
	void close();
private:
	void executeInsert(std::string& sql) const;

	void createUnitsTable() const;
	void createTable(const std::string& sql) const;
	void createBuildingsTable() const;
	void createResourceEntitiesTable() const;
	void createPlayerTable() const;
	void createConfigTable() const;
	void createResourceTable() const;

	sqlite3* database;
	int precision;

	Loading loadingState;
};
