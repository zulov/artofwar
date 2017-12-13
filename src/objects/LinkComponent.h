#pragma once
#include <Urho3D/Scene/Component.h>
#include "Entity.h"
#include "Physical.h"

class LinkComponent :public Urho3D::Component
{
public:
	LinkComponent(Context* context);
	~LinkComponent();

	void bound(Node* _node, Physical* _physical);
	Physical* getPhysical();
	Node* getNode();
private:
	Node* node;
	Physical* physical;
};
