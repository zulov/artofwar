#include "SimulationInfo.h"


SimulationInfo::SimulationInfo() {
	unitDied = &flags[0];
	amountUnitChanged = &flags[1];
	buildingDied = &flags[2];
	amountBuildingChanged = &flags[3];
	resourceDied = &flags[4];
	amountResourceChanged = &flags[5];
	reset();
}


SimulationInfo::~SimulationInfo() = default;

bool SimulationInfo::ifUnitDied() {
	return *unitDied;
}

bool SimulationInfo::ifUnitAmountChanged() {
	return *amountUnitChanged;
}

void SimulationInfo::reset() {
	*unitDied = false;
	*amountUnitChanged = false;

	*buildingDied = false;
	*amountBuildingChanged = false;

	*resourceDied = false;
	*amountResourceChanged = false;
}

void SimulationInfo::setUnitDied() {
	*this->unitDied = true;
	*this->amountUnitChanged = true;
}

void SimulationInfo::setAmountUnitChanged() {
	*this->amountUnitChanged = true;
}

void SimulationInfo::setBuildingDied() {
	*this->buildingDied = true;
	*this->amountBuildingChanged = true;
}

void SimulationInfo::setAmountBuildingChanged() {
	*this->amountBuildingChanged = true;
}

void SimulationInfo::setResourceDied() {
	*this->resourceDied = true;
	*this->amountResourceChanged = true;
}

void SimulationInfo::setAmountResourceChanged() {
	*this->amountResourceChanged = true;
}

void SimulationInfo::set(SimulationInfo* simulationInfo) {
	for (int i = 0; i < SIMULATION_INFO_SIZE; ++i) {
		if (simulationInfo->flags[i]) {
			this->flags[i] = true;
		}
	}
}

bool SimulationInfo::ifUnitDied() const {
	return *unitDied;
}

bool SimulationInfo::ifAmountUnitChanged() const {
	return *amountUnitChanged;
}

bool SimulationInfo::ifBuildingDied() const {
	return *buildingDied;
}

bool SimulationInfo::ifAmountBuildingChanged() const {
	return *amountBuildingChanged;
}

bool SimulationInfo::ifResourceDied() const {
	return *resourceDied;
}

bool SimulationInfo::ifAmountResourceChanged() const {
	return *amountResourceChanged;
}
