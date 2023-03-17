#pragma once
#include <Urho3D/Math/Vector4.h>

struct CameraInfo {
	Urho3D::String info;
	Urho3D::Vector4 boundary;
	bool hasMoved = true;
};
