#pragma once
#include "CameraBehave.h"
#include <vector>

using namespace Urho3D;

class CameraManager {
public:
	CameraManager();
	~CameraManager();
	void setCameraBehave(int _type);
	Camera *getComponent() const;
	void translate(const IntVector2& cursorPos, Input* input, float timeStep) const;
	String *getInfo() const;
	MouseMode getMouseMode() const;
	void rotate(const IntVector2& mouse_move) const;
	void changePosition(float x, float y) const;
private:
	void createCameraKeys(Input* input, bool cameraKeys[4], const IntVector2& cursorPos) const;

	const double MOUSE_SENSITIVITY = 0.1f;
	std::vector<CameraBehave*> cameraBehaves;
	CameraBehave* activeBehave;
	float MOVE_SPEED = 20;

	int widthEdge;
	int heightEdge;

	int widthEdgeMax;
	int heightEdgeMax;
};

