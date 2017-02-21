#pragma once

#include <Urho3D/Engine/Application.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Graphics/Zone.h>
#include "CameraManager.h"
#include "Control.h"
#include "Hud.h"
#include "CameraEnums.h"
#include <Urho3D/Graphics/Octree.h>

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
	void InitMouseMode(MouseMode mode);
	void SetupViewport();

	void createZone();
	void CreateScene();
	void createCamera();
	void createLight();
	SharedPtr<Scene> scene;
	float yaw_;
	float pitch_;

	MouseMode useMouseMode_;

	CameraManager * cameraManager;

private:
	void SetWindowTitleAndIcon();
	void CreateConsoleAndDebugHud();
	void HandleMouseModeRequest(StringHash eventType, VariantMap& eventData);
	void HandleMouseModeChange(StringHash eventType, VariantMap& eventData);
	void HandleKeyDown(StringHash eventType, VariantMap& eventData);
	void HandleKeyUp(StringHash eventType, VariantMap& eventData);

	bool paused;
	Hud * hud;
};
