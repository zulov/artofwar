#include "DatabaseCache.h"
#include "db_strcut.h"
#include "db_utils.h"
#include <iostream>
#include <sqlite3/sqlite3.h>
#include <sstream>


static unsigned fromHex(char** argv, int index) {
	unsigned x;
	std::stringstream ss;
	ss << std::hex << argv[index];
	ss >> x;
	return x;
}

int static loadUnits(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container *>(data);
	const int id = atoi(argv[0]);
	xyz->units[id] = new db_unit(id, argv[1], atoi(argv[2]), atoi(argv[3]), argv[4],
	                             atoi(argv[5]));
	xyz->units_size++;
	return 0;
}

int static loadHudSizes(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container *>(data);
	const int id = atoi(argv[0]);
	xyz->hudSizes[id] = new db_hud_size(id, argv[1]);
	xyz->hud_size_size++;
	return 0;
}

int static loadGraphSettings(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container *>(data);
	const int id = atoi(argv[0]);
	xyz->graphSettings[id] = new db_graph_settings(id, atoi(argv[1]), argv[2], atoi(argv[3]),
	                                               atof(argv[4]), atof(argv[5]), argv[6],
	                                               atoi(argv[7]), atoi(argv[8]), atoi(argv[9]));
	xyz->graph_settings_size++;
	return 0;
}

int static loadBuildings(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container *>(data);
	const int id = atoi(argv[0]);
	xyz->buildings[id] = new db_building(id, argv[1], atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), argv[5]);
	xyz->building_size++;
	return 0;
}

int static loadNation(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container *>(data);
	const int id = atoi(argv[0]);
	xyz->nations[id] = new db_nation(id, argv[1]);
	xyz->nation_size++;
	return 0;
}

int static loadResource(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container *>(data);
	const int id = atoi(argv[0]);
	xyz->resources[id] = new db_resource(id, argv[1], argv[2], atoi(argv[3]), argv[4], argv[5], atof(argv[6]),
	                                     atoi(argv[7]), atoi(argv[8]), atoi(argv[9]), fromHex(argv, 10));
	xyz->resource_size++;

	return 0;
}

int static loadHudVars(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container *>(data);
	const int id = atoi(argv[0]);
	xyz->hudVars[id] = new db_hud_vars(id, atoi(argv[1]), argv[2], atof(argv[3]));
	xyz->hud_vars_size++;

	return 0;
}

int static loadBuildingToUnit(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container *>(data);
	const int buildingId = atoi(argv[1]);
	xyz->unitsForBuilding[buildingId]->push_back(xyz->units[atoi(argv[2])]);

	return 0;
}

int static loadCostUnit(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container *>(data);
	const int unitId = atoi(argv[3]);
	const int resourceId = atoi(argv[1]);
	db_resource* dbResource = xyz->resources[resourceId];
	xyz->costForUnit[unitId]->push_back(new db_cost(atoi(argv[0]), resourceId, atoi(argv[2]), dbResource->name, unitId));

	return 0;
}

int static loadCostUnitLevel(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container *>(data);
	const int unit = atoi(argv[0]);
	const int level = atoi(argv[1]);
	const int resourceId = atoi(argv[2]);
	db_resource* dbResource = xyz->resources[resourceId];

	xyz->costForUnitLevel[unit]->at(level)->push_back(new db_cost(-1, resourceId, atoi(argv[3]), dbResource->name, unit));

	return 0;
}

int static loadCostBuildingLevel(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container *>(data);
	const int building = atoi(argv[0]);
	const int level = atoi(argv[1]);
	const int resourceId = atoi(argv[2]);
	db_resource* dbResource = xyz->resources[resourceId];

	xyz->costForBuildingLevel[building]->at(level)->push_back(new db_cost(-1, resourceId, atoi(argv[3]), dbResource->name,
	                                                                      building));

	return 0;
}

int static loadCostBuilding(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container *>(data);
	const int buildingId = atoi(argv[3]);
	const int resourceId = atoi(argv[1]);
	db_resource* dbResource = xyz->resources[resourceId];
	xyz->costForBuilding[buildingId]->push_back(new db_cost(atoi(argv[0]), resourceId, atoi(argv[2]), dbResource->name,
	                                                        buildingId));

	return 0;
}

int static loadOrders(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container *>(data);
	const int id = atoi(argv[0]);
	xyz->orders[id] = new db_order(id, argv[1], argv[2]);
	xyz->orders_size++;

	return 0;
}

int static loadOrdersToUnit(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container *>(data);
	const int orderId = atoi(argv[2]);
	const int unitId = atoi(argv[1]);
	db_order* dbOrder = xyz->orders[orderId];
	xyz->ordersToUnit[unitId]->push_back(dbOrder);

	return 0;
}

int static loadMap(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container *>(data);
	const int id = atoi(argv[0]);

	xyz->maps[id] = new db_map(id, argv[1], argv[2], atof(argv[3]), atof(argv[4]), argv[5]);
	xyz->maps_size++;
	return 0;
}

int static loadPlayerColors(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container *>(data);
	const int id = atoi(argv[0]);
	xyz->playerColors[id] = new db_player_colors(id, fromHex(argv, 1), fromHex(argv, 2), argv[3]);
	xyz->player_colors_size++;
	return 0;
}

int static loadResolution(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container *>(data);
	const int id = atoi(argv[0]);
	xyz->resolutions[id] = new db_resolution(id, atoi(argv[1]), atoi(argv[2]));
	xyz->resolutions_size++;
	return 0;
}

int static loadSettings(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container *>(data);

	xyz->settings[0] = new db_settings(atoi(argv[0]), atoi(argv[1]));

	return 0;
}

int static loadBuildingLevels(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container *>(data);
	int unitId = atoi(argv[1]);

	xyz->levelsToBuilding[unitId]->push_back(
	                                         new db_building_level(
	                                                               atoi(argv[0]), atoi(argv[1]), argv[2], argv[3], argv[4],
	                                                               atof(argv[5]), atoi(argv[6])
	                                                              )
	                                        );

	return 0;
}

int static loadUnitLevels(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container *>(data);
	int unitId = atoi(argv[1]);
	xyz->levelsToUnit[unitId]->push_back(
	                                     new db_unit_level(
	                                                       atoi(argv[0]), atoi(argv[1]), argv[2], atof(argv[3]),
	                                                       atof(argv[4]), argv[5], argv[6], atof(argv[7]), atof(argv[8]),
	                                                       atof(argv[9]), atoi(argv[10]), atof(argv[11]), atof(argv[12]),
	                                                       atoi(argv[13]), atof(argv[14]), atof(argv[15]), atof(argv[16]),
	                                                       atof(argv[17]), atof(argv[18]))
	                                    );

	return 0;
}

int static loadUnitUpgrade(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container *>(data);
	int pathId = atoi(argv[1]);
	auto unitUpgrade = new db_unit_upgrade(
	                                                   atoi(argv[0]), pathId, atoi(argv[2]), argv[3],
	                                                   atof(argv[4]), atof(argv[5]), atof(argv[6]), atof(argv[7]),
	                                                   atof(argv[8]), atof(argv[9]), atof(argv[10]), atof(argv[11]),
	                                                   atof(argv[12])
	                                                  );
	xyz->unitUpgrades[pathId]->push_back(unitUpgrade);
	xyz->unitUpgradesPerId[atoi(argv[0])] = unitUpgrade;
	return 0;
}

int static loadUnitUpgradePath(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container *>(data);
	int pathId = atoi(argv[0]);
	for (auto unitUpgrade : *xyz->unitUpgrades[pathId]) {
		unitUpgrade->pathName = Urho3D::String(argv[1]);
	}

	return 0;
}

int static loadUnitToUnitUpgrade(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container *>(data);
	int upgradeId = atoi(argv[0]);
	xyz->unitUpgradesPerId[upgradeId]->units.push_back(xyz->units[atoi(argv[1])]);

	return 0;
}

int static loadUnitUpgradeCost(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container *>(data);
	int upgradeId = atoi(argv[0]);
	const int resourceId = atoi(argv[1]);
	db_resource* dbResource = xyz->resources[resourceId];
	xyz->unitUpgradesCosts[upgradeId]->push_back(new db_cost(-1, resourceId, atoi(argv[2]), dbResource->name, upgradeId));

	return 0;
}

int static loadBuildingToUnitUpgradePath(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container *>(data);
	int buildingId = atoi(argv[0]);
	int pathId = atoi(argv[1]);

	xyz->buildings[buildingId]->unitUpgradePath.push_back(pathId);

	return 0;
}

static int callback(void* data, int argc, char** argv, char** azColName) {
	return 0;
}

void DatabaseCache::execute(const char* sql, int (*load)(void*, int, char**, char**)) const {
	char* error;
	const int rc = sqlite3_exec(database, sql, load, dbContainer, &error);
	ifError(rc, error);
}

DatabaseCache::DatabaseCache() {
	const int rc = sqlite3_open("Data/Database/base.db", &database);
	if (rc) {
		std::cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(database) << std::endl << std::endl;
		sqlite3_close(database);
		return;
	}

	dbContainer = new db_container();

	execute("SELECT * from units", loadUnits);
	execute("SELECT * from hud_size", loadHudSizes);
	execute("SELECT * from graph_settings", loadGraphSettings);
	execute("SELECT * from building", loadBuildings);
	execute("SELECT * from nation", loadNation);
	execute("SELECT * from resource", loadResource);
	execute("SELECT * from hud_size_vars", loadHudVars);
	execute("SELECT * from building_to_unit", loadBuildingToUnit);
	execute("SELECT * from cost_building", loadCostBuilding);
	execute("SELECT * from cost_unit", loadCostUnit);
	execute("SELECT * from orders", loadOrders);
	execute("SELECT * from orders_to_unit", loadOrdersToUnit);
	execute("SELECT * from map", loadMap);
	execute("SELECT * from player_colors", loadPlayerColors);
	execute("SELECT * from resolution", loadResolution);
	execute("SELECT * from settings", loadSettings);
	execute("SELECT * from unit_level order by level", loadUnitLevels);
	execute("SELECT * from building_level order by level", loadBuildingLevels);
	execute("SELECT * from cost_unit_level order by level,unit", loadCostUnitLevel);
	execute("SELECT * from cost_building_level order by level,building", loadCostBuildingLevel);
	execute("SELECT * from unit_upgrade order by level", loadUnitUpgrade);
	execute("SELECT * from unit_upgrade_path", loadUnitUpgradePath);
	execute("SELECT * from unit_to_unit_upgrade", loadUnitToUnitUpgrade);
	execute("SELECT * from unit_upgrade_cost", loadUnitUpgradeCost);
	execute("SELECT * from building_to_unit_upgrade_path", loadBuildingToUnitUpgradePath);

	sqlite3_close(database);
}


DatabaseCache::~DatabaseCache() {
	delete dbContainer;
}


void DatabaseCache::executeSingle(const char* sql) {
	const int rc = sqlite3_open("Data/Database/base.db", &database);
	if (rc) {
		std::cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(database) << std::endl << std::endl;
		sqlite3_close(database);
		return;
	}
	execute(sql, callback);
	sqlite3_close(database);
}

void DatabaseCache::setGraphSettings(int i, db_graph_settings* graphSettings) {
	graphSettings->name = dbContainer->graphSettings[i]->name;
	graphSettings->styles = dbContainer->graphSettings[i]->styles;
	delete dbContainer->graphSettings[i];
	dbContainer->graphSettings[i] = graphSettings;
	Urho3D::String sql = "UPDATE graph_settings";
	sql.Append(" SET hud_size = ").Append(Urho3D::String(graphSettings->hud_size));
	//sql.Append("SET style =").Append(Urho3D::String(graphSettings->hud_size));
	sql.Append(", fullscreen = ").Append(Urho3D::String((int)graphSettings->fullscreen));
	sql.Append(", max_fps =").Append(Urho3D::String(graphSettings->max_fps));
	sql.Append(", min_fps =").Append(Urho3D::String(graphSettings->min_fps));
	sql.Append(", name = ").Append("'" + Urho3D::String(graphSettings->name) + "'");
	sql.Append(", v_sync = ").Append(Urho3D::String((int)graphSettings->v_sync));
	sql.Append(", shadow = ").Append(Urho3D::String((int)graphSettings->shadow));
	sql.Append(", texture_quality =").Append(Urho3D::String(graphSettings->texture_quality));
	sql.Append(" WHERE id =").Append(Urho3D::String(i));

	executeSingle(sql.CString());
}

void DatabaseCache::setSettings(int i, db_settings* settings) {
	settings->graph = 0;
	delete dbContainer->settings[0];
	dbContainer->settings[0] = settings;
	Urho3D::String sql = "UPDATE settings";
	sql.Append(" SET graph = ").Append(Urho3D::String(settings->graph));
	sql.Append(", resolution = ").Append(Urho3D::String(settings->resolution));

	executeSingle(sql.CString());
}

std::optional<db_unit_level*> DatabaseCache::getUnitLevel(int id, int level) const {
	if (dbContainer->levelsToUnit[id]->size() > level) {
		return dbContainer->levelsToUnit[id]->at(level);
	}
	return {};
}

std::optional<db_building_level*> DatabaseCache::getBuildingLevel(int id, int level) const {
	if (dbContainer->levelsToBuilding[id]->size() > level) {
		return dbContainer->levelsToBuilding[id]->at(level);
	}
	return {};
}

std::optional<db_unit_upgrade*> DatabaseCache::getUnitUpgrade(int id, int level) const {
	if (dbContainer->unitUpgrades[id]->size() > level) {
		return dbContainer->unitUpgrades[id]->at(level);
	}
	return {};
}

std::optional<std::vector<db_cost*>*> DatabaseCache::getCostForUnitLevel(short id, int level) const {
	if (dbContainer->costForUnitLevel[id]->size() > level
		&& !dbContainer->costForUnitLevel[id]->at(level)->empty()) {
		return dbContainer->costForUnitLevel[id]->at(level);
	}
	return {};
}

std::optional<std::vector<db_cost*>*> DatabaseCache::getCostForBuildingLevel(short id, int level) const {
	if (dbContainer->costForBuildingLevel[id]->size() > level
		&& !dbContainer->costForBuildingLevel[id]->at(level)->empty()) {
		return dbContainer->costForBuildingLevel[id]->at(level);
	}
	return {};
}

std::optional<std::vector<db_cost*>*> DatabaseCache::getCostForUnitUpgrade(short id, int level) const {
	auto upgrade = dbContainer->unitUpgrades[id]->at(level);

	if (!dbContainer->unitUpgradesCosts[upgrade->id]->empty()) {
		return dbContainer->unitUpgradesCosts[upgrade->id];
	}
	return {};
}
