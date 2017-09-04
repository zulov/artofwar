#include "ActionParameter.h"


ActionParameter::ActionParameter() {
}


ActionParameter::~ActionParameter() {
}

Aims* ActionParameter::getAims() {
	return aims;
}

Urho3D::Vector3* ActionParameter::getAimPosition() {
	return aimPosition;
}

Physical* ActionParameter::getFollowTo() {
	return followTo;
}

void ActionParameter::setAims(Aims* _aims) {
	aims = _aims;
}

void ActionParameter::setAimPosition(Urho3D::Vector3* _aim) {
	aimPosition = _aim;
}

void ActionParameter::setFollowTo(Physical* physical) {
	followTo = physical;
}
