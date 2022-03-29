#pragma once

#include <vector>
#include <Urho3D/Container/Str.h>

#include "Loading.h"

struct dbload_config;

class sqlite3;
struct dbload_building;
struct dbload_container;
struct dbload_player;
struct dbload_resource;
struct dbload_resource_entities;
struct dbload_unit;


class SceneLoader {
public:
	SceneLoader() = default;
	~SceneLoader();

	void load();
	void reset();
	dbload_container* getData() const;
	void createLoad(const Urho3D::String& fileName, bool tryReuse);
	dbload_config* getConfig() const;
	std::vector<dbload_player*>* loadPlayers();
	std::vector<dbload_resource*>* loadResources();

	std::vector<dbload_unit*>* loadUnits();
	std::vector<dbload_building*>* loadBuildings();
	std::vector<dbload_resource_entities*>* loadResourcesEntities();
	void close();
	void end();
private:
	void load(const std::string& sql, int (*load)(void*, int, char**, char**)) const;

	sqlite3* database{};
	dbload_container* dbLoad{};
	Urho3D::String lastLoad = "";
};
