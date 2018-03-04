#include "UpgradeCommandList.h"
#include "commands/creation/CreationCommand.h"


UpgradeCommandList::UpgradeCommandList(SimulationObjectManager* _simulationObjectManager) {
	simulationObjectManager = _simulationObjectManager;
}


UpgradeCommandList::~UpgradeCommandList() = default;

void UpgradeCommandList::setParemeters(AbstractCommand* command) {
	CreationCommand* m = dynamic_cast<CreationCommand *>(command);
	m->setSimulationObjectManager(simulationObjectManager);
}
