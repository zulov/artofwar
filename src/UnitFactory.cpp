#include "UnitFactory.h"
#include <iostream>
using namespace std;

static int callback(void *data, int argc, char **argv, char **azColName) {
	int i;
	fprintf(stderr, "%s: ", (const char*)data);
	for (i = 0; i<argc; i++) {
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

UnitFactory::UnitFactory(): EntityFactory() {
	int rc;
	char* error;

	sqlite3* db;
	rc = sqlite3_open("Data/Database/base.db", &db);
	if (rc) {
		cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_close(db);
	}

	char *sql = "SELECT * from units";

	rc = sqlite3_exec(db, sql, callback, nullptr, &error);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", error);
		sqlite3_free(error);
	}
	else {
		fprintf(stdout, "Operation done successfully\n");
	}
	sqlite3_close(db);
}

UnitFactory::~UnitFactory() {
}

double UnitFactory::getSpecSize(SpacingType spacing) {
	switch (spacing) {
	case CONSTANT:
	case RANDOM:
		return INIT_SPACE;
	}
}

String UnitFactory::getModelName(UnitType unitType) {
	switch (unitType) {
	case WARRIOR:
		return "Cube.mdl";
	case PIKEMAN:
		return "Piramide.mdl";
	case CAVALRY:
		return "Prism.mdl";
	case ARCHER:
		return "Cylinder.mdl";
	case WORKER:
		return "Sphere.mdl";
	}
}

std::vector<Unit*>* UnitFactory::create(unsigned int number, UnitType unitType, Vector3* center, SpacingType spacingType) {
	std::vector<Unit*>* units = new std::vector<Unit *>();
	units->reserve(number);

	Game* game = Game::get();
	Font* font = game->getCache()->GetResource<Font>("Fonts/Anonymous Pro.ttf");

	int yMax = number / sqrt(number);

	double space = getSpecSize(spacingType);
	String modelName = "Models/" + getModelName(unitType);
	int produced = 0;
	int y = 0;
	while (produced < number) {
		for (int x = 0; x < yMax; ++x) {
			Vector3* position = new Vector3(x * space + center->x_, 0 + center->y_, y * space + center->z_);
			Node* node = game->getScene()->CreateChild("Box");
			node->SetPosition(*position);

			StaticModel* boxObject = node->CreateComponent<StaticModel>();

			boxObject->SetModel(game->getCache()->GetResource<Model>(modelName));

			Unit* newUnit = new Unit(position, node, font);
			units->push_back(newUnit);
			++produced;
			if (produced >= number) { break; }
		}
		++y;
	}
	return units;
}
