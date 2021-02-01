#pragma once

#include <span>
#include <Urho3D/Engine/Application.h>
#include "Benchmark.h"
#include "GameState.h"
#include "Loading.h"
#include "debug/DebugManager.h"
#include "scene/load/SceneLoader.h"
#include "scene/save/SceneSaver.h"

class SimInfo;

namespace Urho3D {
	class Context;
	enum MouseMode;
}

enum class CameraBehaviorType : char;
struct NewGameForm;
class Controls;
class Hud;
class LevelBuilder;
class SelectedInfo;
class Simulation;
class ObjectsInfo;

class Main : public Urho3D::Application {
URHO3D_OBJECT(Main, Application)

	explicit Main(Urho3D::Context* context);

	void Setup() override;
	void Start() override;
	void Stop() override;
	void HandleUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

protected:

	void InitMouseMode(Urho3D::MouseMode mode);
	void SetupViewport();

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

	SelectedInfo* control(float timeStep, SimInfo* simulationInfo);
	void readParameters();
	void changeCamera(CameraBehaviorType type);
	void load(const Urho3D::String& saveName, Loading& progress);
	void createEnv() const;
	void save(const Urho3D::String& name);
	void running(double timeStep);
	void createSimulation();
	static void setSimpleManagers();
	void updateProgress(Loading& progress, std::string msg) const;
	void newGame(NewGameForm* form, Loading& progress);
	void changeState(GameState newState);

	void writeSum(std::ofstream &outFile, std::span<float> vals, char pId) const;
	void writeOutput() const;

	Urho3D::MouseMode useMouseMode_;
	Simulation* simulation;
	Hud* hud;
	Controls* controls;
	LevelBuilder* levelBuilder;

	Benchmark benchmark;
	SceneSaver saver;
	SceneLoader loader;
	Loading loadingProgress;
	Loading newGameProgress;
	GameState gameState;
	DebugManager debugManager;

	NewGameForm* newGameForm;
	Urho3D::String saveToLoad = "quicksave.db";


	int loadStages = 6;
	int newGamesStages = 6;

	bool inited = false;

	short coefToEdit = 0;

	//
	int timeLimit = 600;
	Urho3D::String outputName = "test.txt";
	Urho3D::String outputType = "";
};
