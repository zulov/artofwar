#include "SimulationCommandList.h"


SimulationCommandList::SimulationCommandList(SimulationObjectManager* _simulationObjectManager) {
	simulationObjectManager = _simulationObjectManager;
}

SimulationCommandList::~SimulationCommandList() {
}

void SimulationCommandList::setParemeters(AbstractCommand* command) {
	CreationCommand* m = static_cast<CreationCommand *>(command);
	m->setSimulationObjectManager(simulationObjectManager);
}
