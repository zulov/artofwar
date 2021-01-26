#pragma once
#include <Urho3D/Math/Vector4.h>

#include "CameraInfo.h"

namespace Urho3D {
	class IntVector2;
	class Vector2;
	class Input;
	class Camera;
	class String;
	enum MouseMode;
}

enum class CameraBehaviorType : char;
class CameraBehave;

class CameraManager {
public:
	CameraManager();
	~CameraManager();
	void setCameraBehave(CameraBehaviorType _type);
	Urho3D::Camera* getComponent() const;
	void translate(const Urho3D::IntVector2& cursorPos, Urho3D::Input* input, float timeStep);
	const Urho3D::String& getPosInfo() const;
	Urho3D::MouseMode getMouseMode() const;

	void changePosition(float x, float y);
	const Urho3D::Vector2 getTargetPos() const;
	const CameraInfo* getCamInfo(float radius);
private:
	void createCameraKeys(Urho3D::Input* input, bool cameraKeys[4], const Urho3D::IntVector2& cursorPos) const;

	CameraBehave* cameraBehaves[3] = {nullptr, nullptr, nullptr};
	CameraBehave* activeBehave;

	CameraInfo* camInfo;
	bool hasMoved = true;

	const float MOUSE_SENSITIVITY = 0.1f;
	float MOVE_SPEED = 20;

	int widthEdge;
	int heightEdge;

	int widthEdgeMax;
	int heightEdgeMax;
};
