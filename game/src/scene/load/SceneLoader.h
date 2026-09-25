#pragma once

#include <string>
#include <Urho3D/Container/Str.h>

#include "Progress.h"
#include "database/db_utils.h"
#include "scene/save/SaveTable.h"

struct dbload_config;

struct sqlite3;
struct dbload_building;
struct dbload_container;
struct dbload_player;
struct dbload_resource;
struct dbload_unit;

class SceneLoader {
public:
	SceneLoader() = default;
	~SceneLoader();

	void load();
	void reset();
	dbload_container* getData() const;
	void createLoad(const Urho3D::String& fileName, bool tryReuse);
	bool hasError() const { return !error.empty(); }
	const std::string& getError() const { return error; }
	dbload_config* getConfig() const;
	const std::vector<dbload_player*>* loadPlayers() const;

	void loadUnits() const;
	void loadBuildings() const;
	void loadResourcesEntities() const;

	void close();
	void end();

private:
	template <class Creator>
	bool count(const std::string& tableName, Creator createFn) const {
		std::string detail;
		if (loadFromTable(database, "SELECT count(*) FROM " + tableName, createFn, &detail)) {
			return true;
		}
		reportError("load '" + path + "': count table '" + tableName + "' failed: " + detail);
		return false;
	}

	template <typename Col, class Creator>
	bool loadSaveTable(const std::string& suffix, Creator createFn) const {
		const auto sql = saveSelectSql<Col>(suffix);
		std::string detail;
		if (loadFromTable(database, sql, createFn, &detail)) {
			return true;
		}
		reportError("load '" + path + "': table '" + SaveTable<Col>::name + "' failed: " + detail);
		return false;
	}

	template <typename Col, class Creator>
	bool loadOptionalSaveTable(const std::string& suffix, Creator createFn) const {
		if (hasTable(SaveTable<Col>::name)) {
			return loadSaveTable<Col>(suffix, createFn);
		}
		return true;
	}

	bool hasTable(const char* tableName) const;
	void loadAimPaths() const;
	void loadRuntimeState() const;
	void reportError(const std::string& message) const;

	sqlite3* database{};
	dbload_container* dbLoad{};
	Urho3D::String lastLoad = "";
	std::string path;
	mutable std::string error;
};
