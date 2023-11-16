#pragma once

#include <span>
#include <Urho3D/Engine/Application.h>
#include "Benchmark.h"
#include "GameState.h"
#include "Loading.h"
#include "control/HealthBarProvider.h"
#include "debug/DebugManager.h"
#include "scene/load/SceneLoader.h"
#include "scene/save/SceneSaver.h"

struct FrameInfo;

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

	SelectedInfo* control(float timeStep, FrameInfo* frameInfo);
	void readParameters();
	void miniReadParameters() const;
	void changeCamera(CameraBehaviorType type);
	void load(const Urho3D::String& saveName, NewGameForm* form);
	void createEnv(unsigned short mainMapResolution) const;
	void save(const Urho3D::String& name);
	void running(float timeStep);
	void createSimulation();
	static void setSimpleManagers();
	void updateProgress(Loading& progress) const;
	void changeState(GameState newState);

	void writeOutput(std::initializer_list<const std::function<float(Player*)>> funcs1,
	                 std::initializer_list<const std::function<std::span<float>(Player*)>> funcs2) const;
	void writeOutput() const;
	void setCameraPos() const;


	Simulation* simulation{};
	Hud* hud{};
	Controls* controls{};
	LevelBuilder* levelBuilder{};

	Benchmark benchmark;
	SceneSaver saver;
	SceneLoader loader;
	Loading loadingProgress;
	HealthBarProvider healthBarProvider;
	DebugManager debugManager;
	
	GameState gameState;
	Urho3D::MouseMode useMouseMode_;

	bool inited = false;
	short coefToEdit = 0;
	int timeLimit = 600;

	NewGameForm* newGameForm;
	Urho3D::String saveToLoad = "quicksave.db";

	Urho3D::String outputName = "test.txt";

};
