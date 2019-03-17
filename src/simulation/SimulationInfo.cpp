#include "SimulationInfo.h"
#include "ObjectEnums.h"

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

void SimulationInfo::reset() const {
	*unitDied = false;
	*amountUnitChanged = false;

	*buildingDied = false;
	*amountBuildingChanged = false;

	*resourceDied = false;
	*amountResourceChanged = false;
}

void SimulationInfo::setUnitDied() const {
	*this->unitDied = true;
	*this->amountUnitChanged = true;
}

void SimulationInfo::setAmountUnitChanged() const {
	*this->amountUnitChanged = true;
}

void SimulationInfo::setBuildingDied() const {
	*this->buildingDied = true;
	*this->amountBuildingChanged = true;
}

void SimulationInfo::setAmountBuildingChanged() const {
	*this->amountBuildingChanged = true;
}

void SimulationInfo::setResourceDied() const {
	*this->resourceDied = true;
	*this->amountResourceChanged = true;
}

void SimulationInfo::setAmountResourceChanged() const {
	*this->amountResourceChanged = true;
}

void SimulationInfo::setSthDied(ObjectType type) const {
	switch (type) {
	case ObjectType::UNIT:
		setUnitDied();
		break;
	case ObjectType::BUILDING:
		setBuildingDied();
		break;
	case ObjectType::RESOURCE:
		setResourceDied();
		break;
	default: ;
	}
}

void SimulationInfo::set(SimulationInfo& simulationInfo) {
	for (int i = 0; i < SIMULATION_INFO_SIZE; ++i) {
		if (simulationInfo.flags[i]) {
			this->flags[i] = true;
		}
	}
}

void SimulationInfo::setUnitsNumber(int _untisNumber) {
	unitsNumber = _untisNumber;
}

int SimulationInfo::getUnitsNumber() const {
	return unitsNumber;
}

void SimulationInfo::setCurrentFrame(int currentFrameNumber) {
	this->currentFrameNumber = currentFrameNumber;
}
