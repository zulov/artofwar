#pragma once
#include "sqlite3/sqlite3.h"
#include "db_strcut.h"
#include <vector>
#include <optional>
#include <string>


class DatabaseCache {
public:
	DatabaseCache(const char* path);
	~DatabaseCache();
	void execute(const char* sql, int (* load)(void*, int, char**, char**)) const;
	bool openDatabase();
	db_unit* getUnit(int i) const { return dbContainer->units[i]; }
	db_hud_size* getHudSize(int i) const { return dbContainer->hudSizes[i]; }
	db_graph_settings* getGraphSettings(int i) const { return dbContainer->graphSettings[i]; }
	db_building* getBuilding(int i) const { return dbContainer->buildings[i]; }
	db_nation* getNation(int i) const { return dbContainer->nations[i]; }
	db_resource* getResource(int i) const { return dbContainer->resources[i]; }
	db_hud_vars* getHudVar(int i) const { return dbContainer->hudVars[i]; }
	db_order* getOrder(int i) const { return dbContainer->orders[i]; }
	db_map* getMap(int i) const { return dbContainer->maps[i]; }
	db_player_colors* getPlayerColor(int i) const { return dbContainer->playerColors[i]; }
	db_settings* getSettings() const { return dbContainer->settings[0]; }
	db_resolution* getResolution(int id) const { return dbContainer->resolutions[id]; }

	std::optional<db_unit_level*> getUnitLevel(int id, int level) const;
	std::optional<db_building_level*> getBuildingLevel(int id, int level) const;
	std::optional<db_unit_upgrade*> getUnitUpgrade(int id, int level) const;
	std::optional<std::vector<db_cost*>*> getCostForUnitLevel(short id, int level) const;
	std::optional<std::vector<db_cost*>*> getCostForBuildingLevel(short id, int level) const;
	std::optional<std::vector<db_cost*>*> getCostForUnitUpgrade(short id, int level) const;


	std::vector<db_unit_level*>* getUnitLevels(int id) const { return dbContainer->levelsToUnit[id]; }
	std::vector<db_building_level*>* getBuildingLevels(int id) const { return dbContainer->levelsToBuilding[id]; }
	std::vector<db_unit_upgrade*>* getUnitUpgrades(int id) const { return dbContainer->unitUpgrades[id]; }
	std::vector<db_unit*>* getUnitsForBuilding(int id) const { return dbContainer->unitsForBuilding[id]; }
	std::vector<db_cost*>* getCostForUnit(int id) const { return dbContainer->costForUnit[id]; }
	std::vector<db_cost*>* getCostForBuilding(int id) const { return dbContainer->costForBuilding[id]; }
	std::vector<db_order*>* getOrdersForUnit(int id) const { return dbContainer->ordersToUnit[id]; }

	std::vector<db_building*>* getBuildingsForNation(int id) const { return dbContainer->buildingsPerNation[id]; }
	std::vector<db_unit*>* getUnitsForNation(int id) const { return dbContainer->unitsPerNation[id]; }

	int getResourceSize() const { return dbContainer->resource_size; }
	int getHudVarsSize() const { return dbContainer->hud_vars_size; }
	int getBuildingSize() const { return dbContainer->building_size; }
	int getUnitSize() const { return dbContainer->units_size; }
	int getOrdersSize() const { return dbContainer->orders_size; }
	int getMapSize() const { return dbContainer->maps_size; }
	int getPlayerColorsSize() const { return dbContainer->player_colors_size; }
	int getNationSize() const { return dbContainer->nation_size; }
	int getHudSizeSize() const { return dbContainer->hud_size_size; }
	int getGraphSettingsSize() const { return dbContainer->graph_settings_size; }
	int getResolutionSize() const { return dbContainer->resolutions_size; }

	void executeSingle(const char* sql);
	void setGraphSettings(int i, db_graph_settings* graphSettings);
	void setSettings(int i, db_settings* settings);

private:
	db_container* dbContainer;
	sqlite3* database;
	std::string base;
};
