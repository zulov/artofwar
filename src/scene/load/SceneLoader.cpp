#include "SceneLoader.h"
#include <sqlite3/sqlite3.h>
#include <ostream>
#include <iostream>
#include "Loading.h"


SceneLoader::SceneLoader() {
	loadingState = new loading();
}


SceneLoader::~SceneLoader() {
	delete loadingState;
}

void SceneLoader::createLoad(Urho3D::String fileName) {
	loadingState->reset(3, "start loading");
	std::string name = std::string("saves/") + fileName.CString() + ".db";
	int rc = sqlite3_open(name.c_str(), &database);
	if (rc) {
		std::cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(database) << std::endl << std::endl;
		sqlite3_close(database);
	}
}
