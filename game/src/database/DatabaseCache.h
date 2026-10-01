#pragma once

#include <string>
#include <vector>
#include <sqlite3/sqlite3.h>

#include "db_container.h"
#include "scene/save/SQLConsts.h"

struct db_unit;
struct db_building;
struct db_nation;
struct db_resource;
struct db_player_colors;

class DatabaseCache {
public:
	explicit DatabaseCache();
	~DatabaseCache();

	void loadBasic(const std::string& name);
	void loadData(const std::string& name);
	void loadMaps(const std::string& name);

	bool openDatabase(const std::string& name, bool readOnly = true);
	const std::vector<db_hud_size*>& getHudSizes() const { return container->hudSizes; }
	const std::vector<db_hud_vars*>& getHudVars() const { return container->hudVars; }
	const std::vector<db_graph_settings*>& getGraphSettings() const { return container->graphSettings; }
	db_settings* getSettings() const { return container->settings; }
	db_resolution* getResolution(int id) const { return container->resolutions[id]; }
	const std::vector<db_resolution*>& getResolutions() const { return container->resolutions; }

	const std::vector<db_map*>& getMaps() const { return container->maps; }

	db_unit* getUnit(unsigned short i) const { return container->units[i]; }
	const std::vector<db_unit*>& getUnits() const { return container->units; }

	db_building* getBuilding(unsigned short i) const { return container->buildings[i]; }
	const std::vector<db_building*>& getBuildings() const { return container->buildings; }

	const std::vector<db_nation*>& getNations() const { return container->nations; }
	db_nation* getNation(unsigned short i) const { return container->nations[i]; }

	db_resource* getResource(unsigned short i) const { return container->resources[i]; }

	const std::vector<db_player_colors*>& getPlayerColors() const { return container->playerColors; }
	db_player_colors* getPlayerColor(int i) const { return container->playerColors[i]; }
	const db_world_age_catalog* getWorldAgeCatalog() const { return &container->worldAgeCatalog; }

	const std::vector<db_unit_level*>& getUnitLevels() const { return container->unitsLevels; }
	const std::vector<db_building_level*>& getBuildingLevels() const { return container->buildingsLevels; }

	void setGraphSettings(int i, db_graph_settings* gs);
	void setSettings(db_settings* settings);
	void refreshAfterParametersRead() const;
	int getResourcesSize() const { return container->resources.size(); }

private:
	template <class Creator>
	bool load(const std::string& tableName, Creator createFn) const {
		return loadFromTable(database, SQLConsts::SELECT + tableName, createFn);
	}
	void validateWorldAgeCatalog() const;
	db_container* container;
	sqlite3* database;
	std::string pathStr;
};
