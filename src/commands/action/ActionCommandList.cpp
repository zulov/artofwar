#include "ActionCommandList.h"
#include "ActionCommand.h"


ActionCommandList::ActionCommandList(AimContainer * _aimContainer) {
	aimContainer = _aimContainer;
}

ActionCommandList::~ActionCommandList() = default;

void ActionCommandList::setParemeters(AbstractCommand* command) {
	ActionCommand* m = dynamic_cast<ActionCommand *>(command);
	m->setAimConteiner(aimContainer);
}
