#include "ActionParameter.h"


ActionParameter::ActionParameter() = default;


ActionParameter::~ActionParameter() = default;

Aims* ActionParameter::getAims() {
	return aims;
}

void ActionParameter::setAims(Aims* _aims) {
	aims = _aims;
}
