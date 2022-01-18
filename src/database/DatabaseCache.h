#pragma once

#include <string>
#include <vector>
#include "db_container.h"

class sqlite3;
struct db_unit;
struct db_building;
struct db_nation;
struct db_resource;
struct db_order;
struct db_player_colors;

class DatabaseCache {
public:
	explicit DatabaseCache(std::string postfix);
	~DatabaseCache();

	void loadBasic(const std::string& name);
	void loadData(const std::string& name);
	void loadMaps(const std::string& name);

	void execute(const std::string& sql, int (* load)(void*, int, char**, char**)) const;
	bool openDatabase(const std::string& name);
	std::vector<db_hud_size*>& getHudSizes() const { return dbContainer->hudSizes; }
	std::vector<db_hud_vars*>& getHudVars() const { return dbContainer->hudVars; }
	std::vector<db_graph_settings*>& getGraphSettings() const { return dbContainer->graphSettings; }
	db_settings* getSettings() const { return dbContainer->settings[0]; }
	db_resolution* getResolution(int id) const { return dbContainer->resolutions[id]; }
	std::vector<db_resolution*>& getResolutions() const { return dbContainer->resolutions; }

	std::vector<db_map*>& getMaps() const { return dbContainer->maps; }

	db_unit* getUnit(int i) const { return dbContainer->units[i]; }
	std::vector<db_unit*>& getUnits() const { return dbContainer->units; }

	db_building* getBuilding(int i) const { return dbContainer->buildings[i]; }
	std::vector<db_building*>& getBuildings() const { return dbContainer->buildings; }

	std::vector<db_nation*>& getNations() const { return dbContainer->nations; }
	db_nation* getNation(int i) const { return dbContainer->nations[i]; }

	db_resource* getResource(int i) const { return dbContainer->resources[i]; }
	db_order* getOrder(int i) const { return dbContainer->orders[i]; }

	std::vector<db_player_colors*>& getPlayerColors() const { return dbContainer->playerColors; }
	db_player_colors* getPlayerColor(int i) const { return dbContainer->playerColors[i]; }

	const std::vector<db_unit_level*>& getUnitLevels() const { return dbContainer->unitsLevels; }
	const std::vector<db_building_level*>& getBuildingLevels() const { return dbContainer->buildingsLevels; }

	void executeSingleBasic(const std::string& name, const char* sql);
	void setGraphSettings(int i, db_graph_settings* graphSettings);
	void setSettings(int i, db_settings* settings);
	void refreshAfterParametersRead();

private:
	db_container* dbContainer;
	sqlite3* database;
	std::string pathStr;
};
