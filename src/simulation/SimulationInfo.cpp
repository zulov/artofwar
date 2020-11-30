#include "SimulationInfo.h"
#include "objects/ObjectEnums.h"

SimulationInfo::SimulationInfo() {
	reset();
}


void SimulationInfo::reset() {
	unitDied = false;
	amountUnitChanged = false;

	buildingDied = false;
	amountBuildingChanged = false;

	resourceDied = false;
	amountResourceChanged = false;
}

void SimulationInfo::setUnitDied() {
	unitDied = true;
	amountUnitChanged = true;
}

void SimulationInfo::setAmountUnitChanged() {
	amountUnitChanged = true;
}

void SimulationInfo::setBuildingDied() {
	buildingDied = true;
	amountBuildingChanged = true;
}

void SimulationInfo::setAmountBuildingChanged() {
	amountBuildingChanged = true;
}

void SimulationInfo::setResourceDied() {
	resourceDied = true;
	amountResourceChanged = true;
}

void SimulationInfo::setAmountResourceChanged() {
	amountResourceChanged = true;
}

void SimulationInfo::set(SimulationInfo& simulationInfo) {
	unitDied = simulationInfo.unitDied;
	amountUnitChanged = simulationInfo.amountUnitChanged;
	buildingDied = simulationInfo.buildingDied;
	amountBuildingChanged = simulationInfo.amountBuildingChanged;
	resourceDied = simulationInfo.resourceDied;
	amountResourceChanged = simulationInfo.amountResourceChanged;
}

void SimulationInfo::setCurrentFrame(unsigned char currentFrameNumber) {
	this->currentFrameNumber = currentFrameNumber;
}
