#include "ActionParameter.h"


ActionParameter::ActionParameter() {
}


ActionParameter::~ActionParameter() {
}

Aims* ActionParameter::getAims() {
	return aims;
}

void ActionParameter::setAims(Aims* _aims) {
	aims = _aims;
}
