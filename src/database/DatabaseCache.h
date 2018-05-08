#pragma once
#include "sqlite3/sqlite3.h"
#include "db_strcut.h"
#include <vector>
#include <optional>

using namespace std;

class DatabaseCache
{
public:
	void execute(const char* sql, int (* load)(void*, int, char**, char**)) const;
	DatabaseCache();
	~DatabaseCache();
	db_unit* getUnit(int i) const;
	db_hud_size* getHudSize(int i) const;
	db_graph_settings* getGraphSettings(int i) const;
	db_building* getBuilding(int i) const;
	db_nation* getNation(int i) const;
	db_resource* getResource(int i) const;
	db_hud_vars* getHudVar(int i) const;
	db_order* getOrder(int i) const;
	db_map* getMap(int i) const;
	db_player_colors* getPlayerColor(int i) const;
	db_settings* getSettings() const;
	db_resolution* getResolution(int id) const;
	optional<db_unit_level*> getUnitLevel(int id, int level) const;
	optional<db_building_level*> getBuildingLevel(int id, int level) const;
	optional<db_unit_upgrade*> getUnitUpgrade(int id, int level) const;
	optional<vector<db_cost*>*> getCostForUnitLevel(short id, int level) const;
	optional<vector<db_cost*>*> getCostForBuildingLevel(short id, int level) const;
	optional<vector<db_cost*>*> getCostForUnitUpgrade(short id, int level) const;


	vector<db_unit_level*>* getUnitLevels(int id) const;
	vector<db_building_level*>* getBuildingLevels(int id) const;
	vector<db_unit_upgrade*>*  getUnitUpgrades(int id) const;
	vector<db_unit*>* getUnitsForBuilding(int id) const;
	vector<db_cost*>* getCostForUnit(int id) const;
	vector<db_cost*>* getCostForBuilding(int id) const;
	vector<db_order*>* getOrdersForUnit(int id) const;

	int getResourceSize() const;
	int getHudVarsSize() const;
	int getBuildingSize() const;
	int getUnitSize() const;
	int getOrdersSize() const;
	int getMapSize() const;
	int getPlayerColorsSize() const;
	int getNationSize() const;
	int getHudSizeSize() const;
	int getGraphSettingsSize() const;
	int getResolutionSize() const;

	void executeSingle(const char* sql);
	void setGraphSettings(int i, db_graph_settings* graphSettings);
	void setSettings(int i, db_settings* settings);

private:
	db_container* dbContainer;
	sqlite3* database;
};
