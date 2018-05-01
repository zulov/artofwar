#include "UpgradeCommandList.h"
#include "UpgradeCommand.h"


UpgradeCommandList::UpgradeCommandList(SimulationObjectManager* _simulationObjectManager) {
	simulationObjectManager = _simulationObjectManager;
}


UpgradeCommandList::~UpgradeCommandList() = default;

void UpgradeCommandList::setParemeters(AbstractCommand* command) {
	auto* m = dynamic_cast<UpgradeCommand *>(command);
	m->setSimulationObjectManager(simulationObjectManager);
}
