#pragma once


#include "camera/CameraManager.h"
#include "Control.h"
#include "hud/Hud.h"
#include "Benchmark.h"
#include "control/Controls.h"
#include "simulation/Simulation.h"
#include "scene/LevelBuilder.h"

#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/EngineDefs.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/SceneEvents.h>
#include "GameState.h"

namespace Urho3D {
	class Node;
	class Scene;
	class Sprite;
}

using namespace Urho3D;

class Main : public Application
{
URHO3D_OBJECT(Main, Application);

public:
	Main(Context* context);
	void Setup() override;
	void load();
	void Start() override;
	void Stop() override;
	void HandleUpdate(StringHash eventType, VariantMap& eventData);

protected:
	void InitMouseMode(MouseMode mode);
	void SetupViewport();

private:
	void subscribeToEvents();
	void running(VariantMap& eventData);
	void SetWindowTitleAndIcon();
	void HandleMouseModeRequest(StringHash eventType, VariantMap& eventData);
	void HandleMouseModeChange(StringHash eventType, VariantMap& eventData);
	void HandleUIButtonHoverOn(StringHash, VariantMap& eventData);
	void HandleUIButtonHoverOff(StringHash, VariantMap& eventData);
	void HandleKeyDown(StringHash eventType, VariantMap& eventData);
	void HandleKeyUp(StringHash eventType, VariantMap& eventData);
	void HandleBuildButton(StringHash eventType, VariantMap& eventData);
	void HandleUnitButton(StringHash eventType, VariantMap& eventData);
	void HandleOrdersButton(StringHash eventType, VariantMap& eventData);
	void HandleSelectedButton(StringHash eventType, VariantMap& eventData);
	void HandleQueueButton(StringHash eventType, VariantMap& eventData);

	void control(float timeStep);
	void changeCamera(int type);

	MouseMode useMouseMode_;
	Simulation* simulation;
	Benchmark* benchmark;
	CameraManager* cameraManager;
	Hud* hud;
	Controls* controls;
	LevelBuilder* levelBuilder;
	GameState gameState;

	int loadStage = 0;
};
