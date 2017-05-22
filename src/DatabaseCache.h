#pragma once
#include "sqlite3/sqlite3.h"
#include <iostream>
#include "db_strcut.h"
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
private:
	db_container* dbContainer;
	sqlite3* database;
};
