#pragma once


#include "CameraManager.h"
#include "Control.h"
#include "Hud.h"
#include "Benchmark.h"
#include "Controls.h"
#include "Simulation.h"
#include "LevelBuilder.h"
#include "Mediator.h"
#include "BuildList.h"

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Core/Timer.h>
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/Console.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Engine/EngineDefs.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/UI/Cursor.h>
#include <Urho3D/UI/Sprite.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/Text3D.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Window.h>
#include "Player.h"
#include "PlayersManager.h"


namespace Urho3D {
	class Node;
	class Scene;
	class Sprite;
}

using namespace Urho3D;

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
	void HandleWindowClick(StringHash eventType, VariantMap& eventData);
	void HandleEndWindowClick(StringHash eventType, VariantMap& eventData);

protected:
	void InitMouseMode(MouseMode mode);
	void SetupViewport();

private:
	void SetWindowTitleAndIcon();
	void CreateConsoleAndDebugHud();
	void HandleMouseModeRequest(StringHash eventType, VariantMap& eventData);
	void HandleMouseModeChange(StringHash eventType, VariantMap& eventData);
	void HandleUIButttonHoverOn(StringHash, VariantMap& eventData);
	void HandleUIButttonHoverOff(StringHash, VariantMap& eventData);
	void HandleKeyDown(StringHash eventType, VariantMap& eventData);
	void HandleKeyUp(StringHash eventType, VariantMap& eventData);
	void HandleUIButtton(StringHash eventType, VariantMap& eventData);
	void HandleSelectedButtton(StringHash eventType, VariantMap& eventData);
	void HandleUIList(StringHash eventType, VariantMap& eventData);
	void control(float timeStep);

	MouseMode useMouseMode_;
	Simulation* simulation;
	Benchmark* benchmark;
	CameraManager* cameraManager;
	Hud* hud;
	Controls* controls;
	LevelBuilder* levelBuilder;
	SceneObjectManager * sceneObjectManager;
	Mediator * mediator;
	ActionCommandList * actionCommandList;

};
