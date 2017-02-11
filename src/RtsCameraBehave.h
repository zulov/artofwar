#pragma once
#include "CameraBehave.h"
class RtsCameraBehave : public CameraBehave {
public:
	RtsCameraBehave(Urho3D::Context* context);
	~RtsCameraBehave();
	virtual void translate(bool cameraKeys[], int wheel, float timeStep);
	virtual void rotate();

	virtual void setRotate(const Urho3D::Quaternion& rotation);
private:
	float maxY=100, minY=10;
};

