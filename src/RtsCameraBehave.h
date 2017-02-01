#pragma once
#include "CameraBehave.h"
class RtsCameraBehave : public CameraBehave {
public:
	RtsCameraBehave(Urho3D::Context* context);
	~RtsCameraBehave();
	virtual void translate(Urho3D::Vector3 vector);
	virtual void rotate();

	virtual void setRotate(const Urho3D::Quaternion& rotation);
};

