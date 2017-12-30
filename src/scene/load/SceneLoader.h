#pragma once
#include <Urho3D/Container/Str.h>
#include <sqlite3/sqlite3.h>
#include "dbload_container.h"

class loading;

class SceneLoader
{
public:
	SceneLoader();
	~SceneLoader();
	
	void load();
	void reset();
	dbload_container* getData();
	void createLoad(const Urho3D::String& fileName);
	std::vector<dbload_player*>* loadPlayers();
	std::vector<dbload_resource*>* loadResources();

	std::vector<dbload_unit*>* loadUnits();
	std::vector<dbload_building*>* loadBuildings();
	std::vector<dbload_resource_entities*>* loadResourcesEntities();
	void end();
private:

	void load(char* sql, int (*load)(void*, int, char**, char**));
	int precision;
	loading * loadingState;
	sqlite3* database;
	dbload_container *dbLoad;
	
};

