#include "BuildingFactory.h"


BuildingFactory::BuildingFactory(): EntityFactory() {
	buildings = new std::vector<Building *>();
	buildings->reserve(DEFAULT_VECTOR_SIZE);
}

BuildingFactory::~BuildingFactory() {
	buildings->clear();
	delete buildings;
}

std::vector<Building*>* BuildingFactory::create(int id, Vector3* center, int player, IntVector2 _bucketCords) {
	buildings->clear();

	Vector3* position = new Vector3(*center);

	Building* building = new Building(position, id, player);
	building->setBucketPosition(_bucketCords);
	buildings->push_back(building);

	return buildings;
}
