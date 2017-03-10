#include "LinkComponent.h"


LinkComponent::LinkComponent(Urho3D::Context* context) : Component(context) {

}


LinkComponent::~LinkComponent() {
}

void LinkComponent::bound(Urho3D::Node* _node, Entity* _entity) {
	node = _node;
	entity = _entity;
}

Entity* LinkComponent::getEntity() {
	return entity;
}

Node* LinkComponent::getNode() {
	return node;
}
