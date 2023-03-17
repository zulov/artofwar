#include "UpgradeCommandList.h"
#include "UpgradeCommand.h"


UpgradeCommandList::UpgradeCommandList(SimulationObjectManager* simulationObjectManager)
	: simulationObjectManager(simulationObjectManager) {
}

void UpgradeCommandList::setParameters(AbstractCommand* command) {
	auto* m = dynamic_cast<UpgradeCommand*>(command);
	m->setSimulationObjectManager(simulationObjectManager);
}
