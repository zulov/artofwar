#include "ActionCommandList.h"
#include "ActionCommand.h"


ActionCommandList::ActionCommandList(AimContainer * _aimContainer) {
	aimContainer = _aimContainer;
}

ActionCommandList::~ActionCommandList() {
}

void ActionCommandList::setParemeters(AbstractCommand* command) {
	ActionCommand* m = static_cast<ActionCommand *>(command);
	m->setAimConteiner(aimContainer);
}
