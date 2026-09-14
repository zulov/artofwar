#pragma once

#include <string>
#include <vector>
#include "Progress.h"
#include "SaveTableUtils.h"

namespace Urho3D {
	class String;
}

struct sqlite3;
class Unit;
class ResourceEntity;
class Building;
class Player;
struct PendingCommandSaveData;
struct UnitRuntimeSaveData;

class SceneSaver {
public:
	explicit SceneSaver(int precision);
	~SceneSaver() = default;

	bool createSave(const Urho3D::String& fileName, const std::vector<Unit*>* units,
					const std::vector<Building*>* buildings, const std::vector<ResourceEntity*>* resources,
					const std::vector<Player*>& players, int mapId, int size);
	const std::string& getError() const { return error; }

private:
	bool createDatabase(const Urho3D::String& fileName);
	bool createTable(const std::string& name, const std::string& sql);
	bool saveUnits(const std::vector<Unit*>* units);
	bool saveBuildings(const std::vector<Building*>* buildings);
	bool saveResources(const std::vector<ResourceEntity*>* resources);
	bool savePlayers(const std::vector<Player*>& players);
	bool saveConfig(int mapId, int size);
	bool saveRuntimeState(const std::vector<Unit*>* units, const std::vector<Building*>* buildings,
						  const std::vector<Player*>& players);
	bool saveUnitOrders(const std::vector<UnitRuntimeSaveData>& unitStates);
	bool saveAimPaths(const std::vector<UnitRuntimeSaveData>& unitStates);
	bool saveQueues(const std::vector<Building*>* buildings, const std::vector<Player*>& players);
	bool savePlayerLevels(const std::vector<Player*>& players);
	bool saveAiState(const std::vector<Player*>& players);
	bool saveAiWants(const std::vector<Player*>& players);
	bool saveAiHistory(const std::vector<Player*>& players);
	bool saveProjectiles();
	bool saveFormations();
	bool saveFormationOrders();
	bool savePendingCommands(const std::vector<PendingCommandSaveData>& commands);
	bool savePendingCommandEntities(const std::vector<PendingCommandSaveData>& commands);
	bool finalizeDatabase();
	void fail(const std::string& message);
	void close();

	template <typename Col, typename WriteRows>
	bool saveRows(WriteRows writeRows) {
		const bool saved = saveTable<Col>(database,
				[this](const std::string& name, const std::string& schema) { return createTable(name, schema); },
				writeRows);
		if (!saved && error.empty()) {
			fail("save '" + path + "': write table '" + std::string(SaveTable<Col>::name) +
					"' failed: " + (database ? sqlite3_errmsg(database) : "database is not open"));
		}
		return saved;
	}

	sqlite3* database{};
	int precision;
	std::string error;
	std::string path;

	Progress savingProgress;
};
