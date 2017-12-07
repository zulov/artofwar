#pragma once
#include <Urho3D/Container/Str.h>
#include <sqlite3/sqlite3.h>
#include "db_load.h"

class loading;

class SceneLoader
{
public:
	SceneLoader();
	~SceneLoader();
	void createLoad(Urho3D::String fileName);
	void end();
private:

	void load(char* sql, int (*load)(void*, int, char**, char**));
	int precision;
	loading * loadingState;
	sqlite3* database;
	db_load *dbLoad;
	
};

