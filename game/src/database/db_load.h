#pragma once
#include <cstdlib>
#include <iosfwd>

#include "db_other_struct.h"
//TODO perf atoi->bool zmienic na ifa
static unsigned fromHex(char** argv, int index) {
	unsigned x;
	std::stringstream ss;
	ss << std::hex << argv[index];
	ss >> x;
	return x;
}

static db_container* getContainer(void* data) { return static_cast<db_container*>(data); }

int static loadUnits(void* data, int argc, char** argv, char** azColName) {
	if (argc == 0) { return 0; }
	setEntity(getContainer(data)->units, new db_unit(atoi(argv[0]), argv[1], argv[2],
	                                                 atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]),
	                                                 atoi(argv[7]), atoi(argv[8]), atoi(argv[9]), atoi(argv[10]),
	                                                 atoi(argv[11]), atoi(argv[12]), atoi(argv[13]), atoi(argv[14]),
	                                                 atoi(argv[15])));
	return 0;
}

int static loadHudSizes(void* data, int argc, char** argv, char** azColName) {
	if (argc == 0) { return 0; }

	getContainer(data)->hudSizes.push_back(new db_hud_size(atoi(argv[0]), argv[1]));

	return 0;
}

int static loadGraphSettings(void* data, int argc, char** argv, char** azColName) {
	if (argc == 0) { return 0; }
	setEntity(getContainer(data)->graphSettings, new db_graph_settings(atoi(argv[0]), atoi(argv[1]), argv[2],
	                                                                   atoi(argv[3]),
	                                                                   atof(argv[4]), atof(argv[5]), argv[6],
	                                                                   atoi(argv[7]), atoi(argv[8]), atoi(argv[9])));
	return 0;
}

int static loadBuildings(void* data, int argc, char** argv, char** azColName) {
	if (argc == 0) { return 0; }
	setEntity(getContainer(data)->buildings, //TODO resource Types zbic do jednego pola
	          new db_building(atoi(argv[0]), argv[1], argv[2],
	                          atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]),
	                          atoi(argv[7]), atoi(argv[8]), atoi(argv[9]), atoi(argv[10]),
	                          atoi(argv[11]), atoi(argv[12]), atoi(argv[13]),
	                          atoi(argv[14]), atoi(argv[15]), atoi(argv[16]),
	                          atoi(argv[17]), atoi(argv[18]),atoi(argv[19])
	          ));
	return 0;
}

int static loadNation(void* data, int argc, char** argv, char** azColName) {
	if (argc == 0) { return 0; }
	setEntity(getContainer(data)->nations, new db_nation(atoi(argv[0]), argv[1], argv[2], argv[3]));
	return 0;
}

int static loadResource(void* data, int argc, char** argv, char** azColName) {
	if (argc == 0) { return 0; }
	const auto resource = new db_resource(atoi(argv[0]), atoi(argv[1]), argv[2], argv[3], atoi(argv[4]),
	                                      argv[5], atoi(argv[6]), atoi(argv[7]), atoi(argv[8]),
	                                      fromHex(argv, 9), atof(argv[10]), atoi(argv[11]) == 1);
	setEntity(getContainer(data)->resources, resource);
	return 0;
}

int static loadHudVars(void* data, int argc, char** argv, char** azColName) {
	if (argc == 0) { return 0; }
	const int id = atoi(argv[0]);
	getContainer(data)->hudVars.push_back(new db_hud_vars(id, atoi(argv[1]), argv[2], atof(argv[3])));

	return 0;
}

int static loadMap(void* data, int argc, char** argv, char** azColName) {
	if (argc == 0) { return 0; }
	setEntity(getContainer(data)->maps, new db_map(atoi(argv[0]), argv[1], argv[2]));
	return 0;
}

int static loadPlayerColors(void* data, int argc, char** argv, char** azColName) {
	if (argc == 0) { return 0; }
	setEntity(getContainer(data)->playerColors,
	          new db_player_colors(atoi(argv[0]), fromHex(argv, 1), fromHex(argv, 2), argv[3]));
	return 0;
}

int static loadResolution(void* data, int argc, char** argv, char** azColName) {
	if (argc == 0) { return 0; }
	setEntity(getContainer(data)->resolutions, new db_resolution(atoi(argv[0]), atoi(argv[1]), atoi(argv[2])));
	return 0;
}

int static loadSettings(void* data, int argc, char** argv, char** azColName) {
	if (argc == 0) { return 0; }
	const auto xyz = getContainer(data);
	ensureSize(1, xyz->settings);
	xyz->settings[0] = new db_settings(atoi(argv[0]), atoi(argv[1]));

	return 0;
}

int static loadBuildingLevels(void* data, int argc, char** argv, char** azColName) {
	if (argc == 0) { return 0; }
	const auto xyz = getContainer(data);
	auto level = new db_building_level(atoi(argv[0]), atoi(argv[1]), atoi(argv[2]), argv[3], argv[4],
	                                   atoi(argv[5]), atoi(argv[6]), atoi(argv[7]), atoi(argv[8]),
	                                   atoi(argv[9]), atoi(argv[10]), atoi(argv[11]), atoi(argv[12]),
	                                   atof(argv[13]), atof(argv[14]), atof(argv[15]), atof(argv[16]),
	                                   atoi(argv[17]), atof(argv[18]), atof(argv[19]), atoi(argv[20]),
	                                   atoi(argv[21]), atoi(argv[22]), atoi(argv[23]));
	setEntity(xyz->buildingsLevels, level);
	xyz->buildings[level->building]->levels.push_back(level);
	for (auto nation : xyz->nations) {
		if (nation) {
			ensureSize(nation->id + 1, level->unitsPerNation);
			ensureSize(nation->id + 1, level->unitsPerNationIds);
			if (level->unitsPerNation[nation->id] == nullptr) {
				level->unitsPerNation[nation->id] = new std::vector<db_unit*>();
			}
			if (level->unitsPerNationIds[nation->id] == nullptr) {
				level->unitsPerNationIds[nation->id] = new std::vector<unsigned char>();
			}
		}
	}
	return 0;
}

int static loadUnitLevels(void* data, int argc, char** argv, char** azColName) {
	if (argc == 0) { return 0; }
	const auto xyz = getContainer(data);

	int unitId = atoi(argv[2]);

	auto level = new db_unit_level(atoi(argv[0]), atoi(argv[1]), unitId, argv[3], argv[4],
	                               atoi(argv[5]), atoi(argv[6]), atoi(argv[7]), atoi(argv[8]),
	                               atoi(argv[9]), atoi(argv[10]), atof(argv[11]),
	                               atof(argv[12]), atof(argv[13]), atof(argv[14]),
	                               atoi(argv[15]), atoi(argv[16]), atof(argv[17]),
	                               atof(argv[18]), atof(argv[19]), atof(argv[20]),
	                               atof(argv[21]), atoi(argv[22]), atof(argv[23]), atof(argv[24]),
	                               atof(argv[25]), atof(argv[26]), atof(argv[27]), atof(argv[28]),
	                               atof(argv[29]), atof(argv[30]), atof(argv[31]));
	setEntity(xyz->unitsLevels, level);
	xyz->units[unitId]->levels.push_back(level);

	return 0;
}

int static loadUnitToNation(void* data, int argc, char** argv, char** azColName) {
	if (argc == 0) { return 0; }
	const auto xyz = getContainer(data);

	auto unit = xyz->units[atoi(argv[0])];
	auto nation = xyz->nations[atoi(argv[1])];
	nation->units.push_back(unit);
	if (unit->typeWorker) {
		nation->workers.push_back(unit);
	}
	unit->nations.push_back(nation);
	return 0;
}

int static loadBuildingToNation(void* data, int argc, char** argv, char** azColName) {
	if (argc == 0) { return 0; }
	const auto xyz = getContainer(data);

	auto building = xyz->buildings[atoi(argv[0])];
	auto nation = xyz->nations[atoi(argv[1])];

	nation->buildings.push_back(building);
	building->nations.push_back(nation);
	return 0;
}

int static loadUnitToBuildingLevels(void* data, int argc, char** argv, char** azColName) {
	if (argc == 0) { return 0; }
	const auto xyz = getContainer(data);

	auto level = xyz->buildingsLevels[atoi(argv[0])];
	auto unit = xyz->units[atoi(argv[1])];
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
