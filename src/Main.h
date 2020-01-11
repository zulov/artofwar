#pragma once

#include "Benchmark.h"
#include "GameState.h"
#include "Loading.h"
#include "control/Controls.h"
#include "hud/Hud.h"
#include "scene/LevelBuilder.h"
#include "scene/load/SceneLoader.h"
#include "scene/save/SceneSaver.h"
#include "simulation/Simulation.h"
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Input/Input.h>
#include "debug/DebugManager.h"


struct NewGameForm;


class Main : public Urho3D::Application
{
URHO3D_OBJECT(Main, Application)

	explicit Main(Urho3D::Context* context);
	void Setup() override;
	void Start() override;
	void Stop() override;
	void HandleUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

protected:
	void InitMouseMode(Urho3D::MouseMode mode);
	void SetupViewport() const;

private:

	void disposeScene();

	void SetWindowTitleAndIcon();

	void subscribeToUIEvents();
	void subscribeToEvents();

	void HandleMouseModeRequest(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleMouseModeChange(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	void HandleKeyDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleKeyUp(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	void HandleNewGame(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleLoadGame(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleCloseGame(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleSaveScene(Urho3D::StringHash, Urho3D::VariantMap& eventData);

	void HandleLeftMenuButton(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleSelectedButton(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	void InitLocalizationSystem() const;

	SelectedInfo* control(float timeStep, SimulationInfo* simulationInfo);
	void changeCamera(int type);
	void load(const Urho3D::String& saveName, loading& progress);
	void createEnv() const;
	void save(const Urho3D::String& name);
	void running(double timeStep);
	void createSimulation();
static void setSimpleManagers();
	void updateProgress(loading& progress, std::string msg) const;
	void newGame(NewGameForm* form, loading& progress);
	void changeState(GameState newState);

	Urho3D::MouseMode useMouseMode_;
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
	DebugManager debugManager;

	NewGameForm* newGameForm;
	Urho3D::String saveToLoad = "quicksave.db";
	int loadStages = 6;
	int newGamesStages = 6;

	bool inited = false;

	short coefToEdit = 0;

};
