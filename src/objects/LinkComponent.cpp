#include "LinkComponent.h"


LinkComponent::LinkComponent(Urho3D::Context* context) : Component(context), physical(nullptr) {
}


LinkComponent::~LinkComponent() = default;

void LinkComponent::bound(Physical* _physical) {
	physical = _physical;
}

Physical* LinkComponent::getPhysical() {
	return physical;
}