#pragma once
#include "Physical.h"
#include <Urho3D/Scene/Component.h>


class LinkComponent :public Urho3D::Component
{
public:
	explicit LinkComponent(Context* context);
	~LinkComponent();

	void bound(Physical* _physical);
	Physical* getPhysical();

private:

	Physical* physical;
};
