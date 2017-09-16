#include "ActionParameter.h"


ActionParameter::ActionParameter() {
}


ActionParameter::~ActionParameter() {
}

Aims* ActionParameter::getAims() {
	return aims;
}

Physical* ActionParameter::getFollowTo() {
	return followTo;
}

void ActionParameter::setAims(Aims* _aims) {
	aims = _aims;
}

void ActionParameter::setVector(Urho3D::Vector3* _aim) {
	vector = _aim;
}

void ActionParameter::setFollowTo(Physical* physical) {
	followTo = physical;
}
