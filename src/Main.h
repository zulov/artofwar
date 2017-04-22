#pragma once

#include <Urho3D/Engine/Application.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Graphics/Zone.h>
#include "CameraManager.h"
#include "Control.h"
#include "Hud.h"
#include "Benchmark.h"
#include "Controls.h"
#include "Simulation.h"
#include "Unit.h"

#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/Graphics/Model.h>
#include "LevelBuilder.h"
#include "Mediator.h"

namespace Urho3D {
	class Node;
	class Scene;
	class Sprite;
}

using namespace Urho3D;

const float TOUCH_SENSITIVITY = 2.0f;

class Main : public Application
{
	// Enable type information.
	URHO3D_OBJECT(Main, Application);

public:
	Main(Context* context);
	virtual void Setup();
	virtual void Start();
	virtual void Stop();
	void HandleUpdate(StringHash eventType, VariantMap& eventData);

protected:
	void InitMouseMode(MouseMode mode);
	void SetupViewport();
	std::vector<Unit*>* createUnits(int size, double space);

	void moveCamera(float timeStep);
	MouseMode useMouseMode_;

private:
	void SetWindowTitleAndIcon();
	void CreateConsoleAndDebugHud();
	void HandleMouseModeRequest(StringHash eventType, VariantMap& eventData);
	void HandleMouseModeChange(StringHash eventType, VariantMap& eventData);
	void HandleKeyDown(StringHash eventType, VariantMap& eventData);
	void HandleKeyUp(StringHash eventType, VariantMap& eventData);

	Simulation* simulation;
	Benchmark* benchmark;
	CameraManager* cameraManager;
	Hud* hud;
	Controls* controls;
	LevelBuilder* levelBuilder;
	SceneObjectManager * sceneObjectManager;
	CommandList * commandList;
	Mediator * mediator;
};
