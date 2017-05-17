#pragma once
#include "sqlite3/sqlite3.h"
#include <iostream>
#include "db_strcut.h"
using namespace std;

class DatabaseCache
{
public:
	DatabaseCache();
	~DatabaseCache();
	db_unit* getUnit(int i);
private:
	db_container* dbContainer;
};
