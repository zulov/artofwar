#pragma once
#include "sqlite3/sqlite3.h"
#include <iostream>
#include "db_strcut.h"
#include "BuildingType.h"
#include <vector>
using namespace std;

class DatabaseCache
{
public:
	void ifError(int rc, char* error);
	void execute(char* sqlUnits, int(* loadUnits)(void*, int, char**, char**), db_container* dbContainer);
	DatabaseCache();
	~DatabaseCache();
	db_unit* getUnit(int i);
	db_hud_size* getHudSize(int i);
	db_graph_settings* getGraphSettings(int i);
	db_building* getBuilding(int i);
	db_building_type* getBuildingType(int i);
	db_unit_type* getUnitType(int i);
	db_nation* getNation(int i);
	db_resource* getResource(int i);
	db_hud_vars* getHudVar(int i);

	int getResourceSize();
	int getBuildingTypeSize();
	int getUnitTypeSize();
	int getHudVarsSize();
	int getBuildingSize();
	int getUnitSize();
	std::vector<db_unit*>* getUnitsForBuilding(int id);

private:
	db_container* dbContainer;
	sqlite3* database;
};
