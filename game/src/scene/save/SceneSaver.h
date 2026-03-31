#pragma once

#include <vector>
#include "Progress.h"

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

	void createSave(const Urho3D::String& fileName, const std::vector<Unit*>* units,
					const std::vector<Building*>* buildings, const std::vector<ResourceEntity*>* resources,
		const std::vector<Player*>& players, int mapId,int size);
private:
	void createDatabase(const Urho3D::String& fileName);
	void createTable(const std::string& name, const std::string& sql);
	void saveUnits(const std::vector<Unit*>* units);
	void saveBuildings(const std::vector<Building*>* buildings);
	void saveResources(const std::vector<ResourceEntity*>* resources);
	void savePlayers(const std::vector<Player*>& players);
	void saveConfig(int mapId, int size);
	void finalizeDatabase();
	void close();

	sqlite3* database;
	int precision;

	Progress savingProgress;
};
