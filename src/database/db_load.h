#pragma once
#include <cstdlib>
#include <ios>
#include <iosfwd>
#include "db_strcut.h"
#include "utils/StringUtils.h"

static unsigned fromHex(char** argv, int index) {
	unsigned x;
	std::stringstream ss;
	ss << std::hex << argv[index];
	ss >> x;
	return x;
}

int static loadUnits(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);

	setEntity(xyz->units, new db_unit(atoi(argv[0]), argv[1], atoi(argv[2]), argv[3], atoi(argv[4])));
	return 0;
}

int static loadHudSizes(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);
	const int id = atoi(argv[0]);
	xyz->hudSizes.push_back(new db_hud_size(id, argv[1]));

	return 0;
}

int static loadGraphSettings(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);

	setEntity(xyz->graphSettings, new db_graph_settings(atoi(argv[0]), atoi(argv[1]), argv[2], atoi(argv[3]),
	                                                    atof(argv[4]), atof(argv[5]), argv[6],
	                                                    atoi(argv[7]), atoi(argv[8]), atoi(argv[9])));
	return 0;
}

int static loadBuildings(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);

	setEntity(xyz->buildings, new db_building(atoi(argv[0]), argv[1], atoi(argv[2]), atoi(argv[3]), argv[4]));
	return 0;
}

int static loadNation(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);

	setEntity(xyz->nations, new db_nation(atoi(argv[0]), argv[1]));
	return 0;
}

int static loadResource(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);

	const auto resource = new db_resource(atoi(argv[0]), argv[1], argv[2], atoi(argv[3]), argv[4],
	                                      atoi(argv[5]), atoi(argv[6]), atoi(argv[7]), fromHex(argv, 8));
	setEntity(xyz->resources, resource);
	return 0;
}

int static loadHudVars(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);
	const int id = atoi(argv[0]);
	xyz->hudVars.push_back(new db_hud_vars(id, atoi(argv[1]), argv[2], atof(argv[3])));

	return 0;
}

static void addCost(char** argv, db_with_cost* withCost) {
	withCost->costs.push_back(new db_cost(atoi(argv[1]), atoi(argv[2])));
}

int static loadCostUnit(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);
	addCost(argv, xyz->units[atoi(argv[0])]);

	return 0;
}

int static loadCostUnitLevel(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);

	addCost(argv, xyz->unitsLevels[atoi(argv[0])]);

	return 0;
}

int static loadCostBuildingLevel(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);

	addCost(argv, xyz->buildingsLevels[atoi(argv[0])]);
	return 0;
}

int static loadCostBuilding(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);

	addCost(argv, xyz->buildings[atoi(argv[0])]);
	return 0;
}

int static loadOrders(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);

	setEntity(xyz->orders, new db_order(atoi(argv[0]), argv[1], argv[2]));
	return 0;
}

int static loadOrdersToUnit(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);
	db_order* dbOrder = xyz->orders[atoi(argv[1])];

	xyz->units[atoi(argv[0])]->orders.push_back(dbOrder);
	xyz->units[atoi(argv[0])]->ordersIds.push_back(dbOrder->id);

	return 0;
}

int static loadMap(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);

	setEntity(xyz->maps, new db_map(atoi(argv[0]), argv[1], argv[2], atof(argv[3]), atof(argv[4]), argv[5]));
	return 0;
}

int static loadPlayerColors(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);

	setEntity(xyz->playerColors, new db_player_colors(atoi(argv[0]), fromHex(argv, 1), fromHex(argv, 2), argv[3]));
	return 0;
}


int static loadResolution(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);

	setEntity(xyz->resolutions, new db_resolution(atoi(argv[0]), atoi(argv[1]), atoi(argv[2])));
	return 0;
}

int static loadSettings(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);
	ensureSize(1, xyz->settings);
	xyz->settings[0] = new db_settings(atoi(argv[0]), atoi(argv[1]));

	return 0;
}


int static loadBuildingLevels(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);
	auto level = new db_building_level(atoi(argv[0]), atoi(argv[1]), atoi(argv[2]), argv[3],
	                                   argv[4], atoi(argv[5]), atof(argv[6]), atoi(argv[7]),
	                                   atoi(argv[8]), atof(argv[9]));
	setEntity(xyz->buildingsLevels, level);
	xyz->buildings[level->building]->levels.push_back(level);
	for (auto nation : xyz->nations) {
		if (nation) {
			level->unitsPerNation.resize(nation->id + 1, nullptr);
			level->unitsPerNationIds.resize(nation->id + 1, nullptr);
			if (level->unitsPerNation[nation->id] == nullptr) {
				level->unitsPerNation[nation->id] = new std::vector<db_unit*>();
			}
			if (level->unitsPerNationIds[nation->id] == nullptr) {
				level->unitsPerNationIds[nation->id] = new std::vector<short>();
			}
		}
	}
	return 0;
}

int static loadUnitLevels(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);
	auto levelId = atoi(argv[0]);
	int unitId = atoi(argv[2]);

	auto level = new db_unit_level(levelId, atoi(argv[1]), atoi(argv[2]), argv[3], atof(argv[4]),
	                               atof(argv[5]), argv[6], atof(argv[7]), atoi(argv[8]),
	                               atof(argv[9]), atof(argv[10]), atof(argv[11]), atof(argv[12]),
	                               atof(argv[13]), atof(argv[14]), atof(argv[15]),
	                               atof(argv[16]), atof(argv[17]), atoi(argv[18]),
	                               atoi(argv[19]), atoi(argv[20]), atof(argv[21]));
	setEntity(xyz->unitsLevels, level);
	xyz->units[unitId]->levels.push_back(level);

	return 0;
}

int static loadUnitToNation(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);
	int unitId = atoi(argv[0]);
	int nationId = atoi(argv[1]);
	xyz->nations[nationId]->units.push_back(xyz->units[unitId]);
	xyz->units[unitId]->nations.push_back(xyz->nations[nationId]);
	return 0;
}

int static loadBuildingToNation(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);
	int buildingId = atoi(argv[0]);
	int nationId = atoi(argv[1]);
	xyz->nations[nationId]->buildings.push_back(xyz->buildings[buildingId]);
	xyz->buildings[buildingId]->nations.push_back(xyz->nations[nationId]);
	return 0;
}

int static loadUnitToBuildingLevels(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);

	auto level = xyz->buildingsLevels[atoi(argv[1])];
	auto unit = xyz->units[atoi(argv[0])];
	level->allUnits.push_back(unit);
	for (auto nation : unit->nations) {
		level->unitsPerNation[nation->id]->push_back(unit);
		level->unitsPerNationIds[nation->id]->push_back(unit->id);
	}

	return 0;
}

static int callback(void* data, int argc, char** argv, char** azColName) {
	return 0;
}

static std::string aiPropsAsString(db_ai_property* aiProps) {
	float output[AI_PROPS_SIZE];
	std::fill_n(output, AI_PROPS_SIZE, 0.f);

	std::copy(aiProps->params, aiProps->params + AI_PROPS_SIZE, output);
	return join(output, output + AI_PROPS_SIZE);
}

static db_ai_property* createAiProp(char** argv, std::vector<db_cost*>& costs, float normDiv) {
	auto prop = new db_ai_property(atof(argv[1]), atof(argv[2]), atof(argv[3]));

	float sum = 0;
	for (auto cost : costs) {
		sum += cost->value;
	}
	prop->setCostSum(sum, normDiv);
	prop->setParamsNormAString(aiPropsAsString(prop));

	return prop;
}

static void addAiProp(char** argv, db_level* level, std::vector<db_cost*>& costs) {
	level->aiProps = createAiProp(argv, costs, 100);
}

static void addAiPropUp(char** argv, db_level* level, std::vector<db_cost*>& costs) {
	level->aiPropsLevelUp = createAiProp(argv, costs, 1000);
}

static int loadAiPropBuildingLevel(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);

	auto level = xyz->buildingsLevels[atoi(argv[0])];

	addAiProp(argv, level, xyz->buildings[level->building]->costs);
	return 0;
}

static int loadAiPropUnitLevel(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);

	auto level = xyz->unitsLevels[atoi(argv[0])];

	addAiProp(argv, level, xyz->units[level->unit]->costs);
	return 0;
}

static int loadAiPropBuildingLevelUp(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);

	auto level = xyz->buildingsLevels[atoi(argv[0])];

	addAiPropUp(argv, level, level->costs);
	return 0;
}

static int loadAiPropUnitLevelUp(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);

	auto level = xyz->unitsLevels[atoi(argv[0])];

	addAiPropUp(argv, level, level->costs);
	return 0;
}
