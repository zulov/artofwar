#include "BuildingOrder.h"

BuildingOrder::BuildingOrder(Building* building, BuildingActionType action, short id)
	: FutureOrder(static_cast<short>(action), id, false) {
	buildings.emplace_back(building);
}

BuildingOrder::BuildingOrder(std::vector<Physical*>* buildings, BuildingActionType action, short id)
	: FutureOrder(static_cast<short>(action), id, false) {
	for (auto building : *buildings) {//TODO performance spróbowaæ z insertem
		this->buildings.emplace_back(reinterpret_cast<Building*>(building));
	}
}
