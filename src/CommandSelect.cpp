#include "CommandSelect.h"


CommandSelect::CommandSelect(std::pair<Entity*, Entity*>* _held) {
	held = _held;
}


CommandSelect::~CommandSelect() {
}

void CommandSelect::execute() {

}
