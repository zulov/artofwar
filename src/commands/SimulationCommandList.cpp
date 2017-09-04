#include "SimulationCommandList.h"


SimulationCommandList::SimulationCommandList(SimulationObjectManager* _simulationObjectManager) {
	simulationObjectManager = _simulationObjectManager;
}

SimulationCommandList::~SimulationCommandList() {
}

void SimulationCommandList::setParemeters(AbstractCommand* command) {
	SimulationCommand* m = static_cast<SimulationCommand *>(command);
	m->setSimulationObjectManager(simulationObjectManager);
}
