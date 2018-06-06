#pragma once
#include <Urho3D/Scene/Component.h>

class Physical;

class LinkComponent : public Urho3D::Component
{
public:
	explicit LinkComponent(Urho3D::Context* context) : Component(context), physical(nullptr) {
	};
	~LinkComponent() = default;

	void bound(Physical* _physical) { physical = _physical; }
	Physical* getPhysical() const { return physical; }
private:
	Physical* physical;
};
