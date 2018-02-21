#pragma once


#include "camera/CameraManager.h"
#include "hud/Hud.h"
#include "Benchmark.h"
#include "control/Controls.h"
#include "simulation/Simulation.h"
#include "scene/LevelBuilder.h"
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include "GameState.h"
#include "Loading.h"
#include "scene/save/SceneSaver.h"
#include "scene/load/SceneLoader.h"

struct NewGameForm;

namespace Urho3D {
	class Node;
	class Scene;
	class Sprite;
}

using namespace Urho3D;

class Main : public Application
{
URHO3D_OBJECT(Main, Application)

	Main(Context* context);
	void Setup() override;
	void Start() override;
	void Stop() override;
	void HandleUpdate(StringHash eventType, VariantMap& eventData);

protected:
	void InitMouseMode(MouseMode mode);
	void SetupViewport();

private:

	void disposeScene();

	void running(const double timeStep);
	void SetWindowTitleAndIcon();

	void subscribeToUIEvents();
	void subscribeToEvents();
	void HandleMouseModeRequest(StringHash eventType, VariantMap& eventData);
	void HandleMouseModeChange(StringHash eventType, VariantMap& eventData);
	void HandleKeyDown(StringHash eventType, VariantMap& eventData);
	void HandleKeyUp(StringHash eventType, VariantMap& eventData);
	void HandleNewGame(StringHash eventType, VariantMap& eventData);
	void HandleLoadGame(StringHash eventType, VariantMap& eventData);
	void HandleCloseGame(StringHash eventType, VariantMap& eventData);
	void HandleSaveScene(StringHash, VariantMap& eventData);
	void HandleMiniMapClick(StringHash eventType, VariantMap& eventData);
	void HandleBuildButton(StringHash eventType, VariantMap& eventData);
	void HandleLeftMenuButton(StringHash eventType, VariantMap& eventData);
	void HandleUnitButton(StringHash eventType, VariantMap& eventData);
	void HandleOrdersButton(StringHash eventType, VariantMap& eventData);
	void HandleSelectedButton(StringHash eventType, VariantMap& eventData);

	void InitLocalizationSystem();

	SelectedInfo* control(float timeStep, SimulationInfo* simulationInfo) const;
	void changeCamera(int type);
	void load();
	void save(String name);
	void createSimulation();
	void newGame(NewGameForm* form);
	void changeState(GameState newState);

	MouseMode useMouseMode_;
	Simulation* simulation;
	Benchmark benchmark;
	Hud* hud;
	Controls* controls;
	LevelBuilder* levelBuilder;
	SceneSaver saver;
	SceneLoader loader;
	loading loadingProgress;
	loading newGameProgress;
	GameState gameState;

	NewGameForm* newGameForm;
	String saveToLoad = "quicksave.db";
	int loadStages = 6;
	int newGamesStages = 6;

	bool inited = false;
};
