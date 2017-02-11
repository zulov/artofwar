#pragma once
#include <vector>
#include "CameraEnums.h"
#include "CameraBehave.h"
#include "FreeCameraBehave.h"
#include "RtsCameraBehave.h"

using namespace Urho3D;

class CameraManager {
public:
	CameraManager(Urho3D::Context* context);
	~CameraManager();
	void setCameraBehave(int _type);
	void setRotation(const Urho3D::Quaternion& rotation);
	Camera *getComponent();
	void translate(bool cameraKeys[], int wheel, float timeStep);
private:

	std::vector<CameraBehave*> cameraBehaves;
	CameraBehave* activeBehave;
	float MOVE_SPEED = 20;
};

