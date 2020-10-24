#pragma once
#include "CameraBehave.h"

enum class CameraBehaviorType : char;
class CameraBehave;

class CameraManager {
public:
	CameraManager();
	~CameraManager();
	void setCameraBehave(CameraBehaviorType _type);
	Urho3D::Camera *getComponent() const;
	void translate(const Urho3D::IntVector2& cursorPos, Urho3D::Input* input, float timeStep) const;
	Urho3D::String *getInfo() const;
	Urho3D::MouseMode getMouseMode() const;
	void rotate(const Urho3D::IntVector2& mouse_move) const;
	void changePosition(float x, float y) const;
private:
	void createCameraKeys(Urho3D::Input* input, bool cameraKeys[4], const Urho3D::IntVector2& cursorPos) const;

	CameraBehave* cameraBehaves[3];
	CameraBehave* activeBehave;
	const float MOUSE_SENSITIVITY = 0.1f;
	float MOVE_SPEED = 20;

	int widthEdge;
	int heightEdge;

	int widthEdgeMax;
	int heightEdgeMax;
};

