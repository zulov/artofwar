#pragma once

#include <Urho3D/Engine/Application.h>
#include <Urho3D/Input/Input.h>
#include "CameraManager.h"
#include "Control.h"

namespace Urho3D {
	class Node;
	class Scene;
	class Sprite;
}

using namespace Urho3D;

const float TOUCH_SENSITIVITY = 2.0f;

class Main : public Application {
	// Enable type information.
	URHO3D_OBJECT(Main, Application);

public:
	Main(Context* context);
	virtual void Setup();
	virtual void Start();
	virtual void Stop();

protected:
	/// Return XML patch instructions for screen joystick layout for a specific sample app, if any.
	virtual String GetScreenJoystickPatchString() const { return String::EMPTY; }
	void InitMouseMode(MouseMode mode);
	void SetupViewport();

	SharedPtr<Scene> scene;

	float yaw_;
	float pitch_;
	bool touchEnabled_;
	MouseMode useMouseMode_;

	CameraManager * cameraManager;

private:
	void CreateLogo();
	void SetWindowTitleAndIcon();
	void CreateConsoleAndDebugHud();
	void HandleMouseModeRequest(StringHash eventType, VariantMap& eventData);
	void HandleMouseModeChange(StringHash eventType, VariantMap& eventData);
	void HandleKeyDown(StringHash eventType, VariantMap& eventData);
	void HandleKeyUp(StringHash eventType, VariantMap& eventData);

	bool paused;


};
