#pragma once
#include "CameraBehave.h"
class FreeCameraBehave : public CameraBehave {
public:
	FreeCameraBehave(Urho3D::Context* context);
	~FreeCameraBehave();

	virtual void translate(Urho3D::Vector3 vector);
	virtual void rotate();
	virtual void setRotate(const Urho3D::Quaternion& rotation);
};

