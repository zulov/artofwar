#include "LinkComponent.h"


LinkComponent::LinkComponent(Urho3D::Context* context) : Component(context) {

}


LinkComponent::~LinkComponent() {
}

void LinkComponent::bound(Urho3D::Node* _node, Physical* _physical) {
	node = _node;
	physical = _physical;
}

Physical* LinkComponent::getPhysical() {
	return physical;
}

Node* LinkComponent::getNode() {
	return node;
}
