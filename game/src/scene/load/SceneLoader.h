#pragma once

#include <Urho3D/Container/Str.h>

#include "Loading.h"

struct dbload_config;

class sqlite3;
struct dbload_building;
struct dbload_container;
struct dbload_player;
struct dbload_resource;
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
	const std::vector<dbload_player*>* loadPlayers() const;

	void loadUnits() const;
	void loadBuildings() const;
	void loadResourcesEntities() const;

	void close();
	void end();
private:
	template <class Creator>
	void load(const std::string& tableName, Creator createFn) const;

	sqlite3* database{};
	dbload_container* dbLoad{};
	Urho3D::String lastLoad = "";
};
