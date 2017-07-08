#pragma once
#include <Urho3D/Scene/Component.h>
#include "Entity.h"
#include "Physical.h"

class LinkComponent :public Urho3D::Component
{
public:
	LinkComponent(Context* context);
	~LinkComponent();

	void bound(Node* _node, Entity* _entity);
	Entity* getEntity();
	Physical* getPhysical();
	Node* getNode();
private:
	Node* node;
	Entity* entity;
	Physical* physical;
};
