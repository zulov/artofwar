#pragma once

#include "Loading.h"
#include "vector"

namespace Urho3D {
	class String;
}

class sqlite3;
struct dbload_building;
struct dbload_container;
struct dbload_player;
struct dbload_resource;
struct dbload_resource_entities;
struct dbload_unit;


class SceneLoader {
public:
	SceneLoader();
	~SceneLoader();

	void load();
	void reset();
	dbload_container* getData() const;
	void createLoad(const Urho3D::String& fileName);
	std::vector<dbload_player*>* loadPlayers();
	std::vector<dbload_resource*>* loadResources();

	std::vector<dbload_unit*>* loadUnits();
	std::vector<dbload_building*>* loadBuildings();
	std::vector<dbload_resource_entities*>* loadResourcesEntities();
	void end();
private:
	void load(const char* sql, int (*load)(void*, int, char**, char**)) const;

	Loading loadingState;
	sqlite3* database;
	dbload_container* dbLoad;
};
