#include "DatabaseCache.h"
#include <sqlite3/sqlite3.h>
#include "db_strcut.h"

int static loadUnits(void* data, int argc, char** argv, char** azColName) {
	db_container* xyz = (db_container *)data;
	int id = atoi(argv[3]);
	xyz->units[id] = new db_unit(argv[0], atof(argv[1]), atof(argv[2]), atoi(argv[3]), argv[4], argv[5], argv[6], atof(argv[7]), atof(argv[8]), atof(argv[9]));//toDO moza sprobowac pêtl¹

	return 0;
}

DatabaseCache::DatabaseCache() {
	int rc;
	char* error;

	sqlite3* database;
	rc = sqlite3_open("Data/Database/base.db", &database);
	if (rc) {
		cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(database) << endl << endl;
		sqlite3_close(database);
	}

	char* sqlUnits = "SELECT * from units";
	char* sqlBuildings = "SELECT * from buildings";
	dbContainer = new db_container();
	rc = sqlite3_exec(database, sqlUnits, loadUnits, dbContainer, &error);
	//rc = sqlite3_exec(database, sqlBuildings, loadBuildings, dbContainer, &error);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", error);
		sqlite3_free(error);
	}
	sqlite3_close(database);
}


DatabaseCache::~DatabaseCache() {
	delete dbContainer;
}

db_unit* DatabaseCache::getUnit(int i) {
	return dbContainer->units[i];
}
