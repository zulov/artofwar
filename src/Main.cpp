#include "Main.h"
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Console.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Engine/EngineDefs.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Resource/Localization.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include "camera/CameraEnums.h"
#include "camera/CameraManager.h"
#include "colors/ColorPaletteRepo.h"
#include "control/Controls.h"
#include "database/DatabaseCache.h"
#include "database/db_grah_structs.h"
#include "hud/Hud.h"
#include "hud/HudData.h"
#include "hud/MySprite.h"
#include "hud/window/in_game_menu/middle/FileFormData.h"
#include "hud/window/main_menu/new_game/NewGameForm.h"
#include "hud/window/selected/SelectedHudElement.h"
#include "math/RandGen.h"
#include "objects/unit/ActionParameter.h"
#include "player/PlayersManager.h"
#include "scene/LevelBuilder.h"
#include "simulation/SimInfo.h"
#include "simulation/Simulation.h"
#include "simulation/env/Environment.h"
#include "simulation/formation/FormationManager.h"
#include "stats/AiInputProvider.h"
#include "stats/Stats.h"


URHO3D_DEFINE_APPLICATION_MAIN(Main)

using namespace Urho3D;

Main::Main(Context* context) : Application(context), useMouseMode_(MM_ABSOLUTE), saver(100),
                               gameState(GameState::STARTING) {
	MySprite::RegisterObject(context);
	Game::init();
	RandGen::init();
}

void Main::Setup() {
	Game::setDatabaseCache(new DatabaseCache(GetSubsystem<FileSystem>()->GetCurrentDir().CString()));
	db_settings* settings = Game::getDatabase()->getSettings();
	db_graph_settings* graphSettings = Game::getDatabase()->getGraphSettings()[settings->graph];
	db_resolution* resolution = Game::getDatabase()->getResolution(settings->resolution);
	engineParameters_[EP_WINDOW_TITLE] = GetTypeName();
	engineParameters_[EP_LOG_NAME] = GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs") + GetTypeName()
		+ ".log";
	engineParameters_[EP_FULL_SCREEN] = graphSettings->fullscreen;
	engineParameters_[EP_HEADLESS] = false;
	engineParameters_[EP_SOUND] = false;
	engineParameters_[EP_WINDOW_WIDTH] = resolution->x;
	engineParameters_[EP_WINDOW_HEIGHT] = resolution->y;
	engineParameters_["ResourcePaths"] = "Data;CoreData;CoreDataMy";
	engineParameters_["ResourcePrefixPaths"] = " ;../";

	engine_->SetMaxFps(graphSettings->max_fps);
	engine_->SetMinFps(graphSettings->min_fps);

	Game::setCache(GetSubsystem<ResourceCache>())
		->setUI(GetSubsystem<UI>())
		->setConsole(GetSubsystem<Console>())
		->setContext(context_)
		->setLog(GetSubsystem<Log>());

	loadingProgress.reset(loadStages);
	newGameProgress.reset(newGamesStages);
}

void Main::Start() {
	Game::setGraphics(GetSubsystem<Graphics>());

	SetWindowTitleAndIcon();
	InitLocalizationSystem();

	subscribeToEvents();

	hud = new Hud();
	hud->prepareUrho(engine_);
	hud->createMyPanels();
	subscribeToUIEvents();
	InitMouseMode(MM_RELATIVE);
	changeState(GameState::LOADING);
}

void Main::Stop() {
	disposeScene();

	delete hud;
	delete Game::getDatabase();
	Game::setDatabaseCache(nullptr);
	Game::dispose();
	RandGen::dispose();
	engine_->DumpResources(true);
}

void Main::subscribeToUIEvents() {
	for (auto hudData : hud->getButtonsLeftMenuToSubscribe()) {
		SubscribeToEvent(hudData->getUIParent(), E_CLICK, URHO3D_HANDLER(Main, HandleLeftMenuButton));
	}

	for (auto buttton : hud->getButtonsSelectedToSubscribe()) {
		SubscribeToEvent(buttton, E_CLICK, URHO3D_HANDLER(Main, HandleSelectedButton));
	}

	SubscribeToEvent(hud->getSaveButton(), E_CLICK, URHO3D_HANDLER(Main, HandleSaveScene));
	SubscribeToEvent(hud->getNewGameProceed(), E_CLICK, URHO3D_HANDLER(Main, HandleNewGame));
	SubscribeToEvent(hud->getLoadButton(), E_CLICK, URHO3D_HANDLER(Main, HandleLoadGame));
	SubscribeToEvent(hud->getCloseButton(), E_CLICK, URHO3D_HANDLER(Main, HandleCloseGame));

	hud->subscribeToUIEvents();
}

void Main::subscribeToEvents() {
	SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(Main, HandleKeyDown));
	SubscribeToEvent(E_KEYUP, URHO3D_HANDLER(Main, HandleKeyUp));
	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Main, HandleUpdate));
}

void Main::running(const double timeStep) {
	Game::addTime(timeStep);
	benchmark.add(1.0f / timeStep);

	SimInfo* simInfo = simulation->update(timeStep);
	debugManager.draw();

	SelectedInfo* selectedInfo = control(timeStep, simInfo);

	hud->update(benchmark, Game::getCameraManager(), selectedInfo, simInfo);
}

void Main::HandleUpdate(StringHash eventType, VariantMap& eventData) {
	switch (gameState) {
	case GameState::MENU_MAIN:
		//changeState(GameState::LOADING);
		break;
	case GameState::LOADING:
		load(saveToLoad, loadingProgress);
		break;
	case GameState::RUNNING:
		running(eventData[SceneUpdate::P_TIMESTEP].GetDouble());
		break;
	case GameState::PAUSE: break;
	case GameState::CLOSING:
		disposeScene();
		changeState(GameState::MENU_MAIN);
		break;
	case GameState::NEW_GAME:
		newGame(newGameForm, newGameProgress);
		break;
	case GameState::STARTING: break;
	default: ;
	}
}

void Main::InitMouseMode(MouseMode mode) {
	useMouseMode_ = mode;
	auto input = GetSubsystem<Input>();

	Console* console = Game::getConsole();
	if (useMouseMode_ != MM_ABSOLUTE) {
		input->SetMouseMode(useMouseMode_);
		if (console && console->IsVisible()) {
			input->SetMouseMode(MM_ABSOLUTE, true);
		}
	}
}

void Main::SetWindowTitleAndIcon() {
	Graphics* graphics = Game::getGraphics();
	const auto icon = Game::getCache()->GetResource<Image>("textures/UrhoIcon.png");
	graphics->SetWindowIcon(icon);
	graphics->SetWindowTitle("Art of War 2017");
}

void Main::changeCamera(CameraBehaviorType type) {
	Game::getCameraManager()->setCameraBehave(type);
	SetupViewport();
	InitMouseMode(Game::getCameraManager()->getMouseMode());
}

void Main::InitLocalizationSystem() const {
	auto l10n = GetSubsystem<Localization>();

	l10n->LoadJSONFile("lang/language.json");
	Game::setLocalization(l10n);
}

void Main::save(const String& name) {
	saver.createSave(name);
	saver.saveConfig();
	simulation->save(saver);
	Game::getPlayersMan()->save(saver);
	saver.close();
}

void Main::createSimulation() {
	simulation = new Simulation(Game::getEnvironment());
}

void Main::setSimpleManagers() {
	Game::setCameraManager(new CameraManager())
		->setFormationManager(new FormationManager())
		->setPlayersManager(new PlayersManager())
		->setAiInputProvider(new AiInputProvider())
		->setStats(new Stats())
		->setColorPaletteRepo(new ColorPaletteRepo());
}

void Main::updateProgress(loading& progress, std::string msg) const {
	progress.inc(std::move(msg));
	hud->updateLoading(progress.getProgress());
}

void Main::load(const String& saveName, loading& progress) {
	switch (progress.currentStage) {
	case 0:
	{
		setSimpleManagers();

		loader.createLoad(saveName);
		levelBuilder = new LevelBuilder();
		controls = new Controls(GetSubsystem<Input>());
		SetupViewport();

		Game::getPlayersMan()->load(loader.loadPlayers(), loader.loadResources());
		Game::getStats()->init();
		controls->init();
		
		subscribeToUIEvents();
		hud->resetLoading();

		levelBuilder->createScene(loader);
	}
	break;
	case 1:
	{
		createEnv();
		break;
	}
	case 2:
	{
		Game::getEnvironment()->prepareGridToFind();
		hud->createMiniMap();
		break;
	}
	case 3:
		createSimulation();
		break;
	case 4:
		simulation->initScene(loader);
		break;
	case 5:
		changeState(GameState::RUNNING);
		loader.end();
		inited = true;
		break;
	}
	updateProgress(progress, Game::getLocalization()->Get("load_msg_" + String(progress.currentStage)).CString());
}

void Main::createEnv() const {
	Game::setEnvironment(new Environment(levelBuilder->getTerrain()));
}

void Main::newGame(NewGameForm* form, loading& progress) {
	switch (progress.currentStage) {
	case 0:
	{
		disposeScene();
		setSimpleManagers();

		levelBuilder = new LevelBuilder();
		controls = new Controls(GetSubsystem<Input>());
		SetupViewport();

		Game::getPlayersMan()->load(form);
		Game::getStats()->init();
		controls->init();

		hud->resetLoading();

		levelBuilder->createScene(form);
	}
	break;
	case 1:
	{
		createEnv();
		break;
	}
	case 2:
	{
		Game::getEnvironment()->prepareGridToFind();
		hud->createMiniMap();
		break;
	}
	case 3:
		createSimulation();
		break;
	case 4:
		simulation->initScene(form);
		break;
	case 5:

		delete form; //TODO trzeba ustawic na null

		changeState(GameState::RUNNING);
		inited = true;
		break;
	}
	updateProgress(progress, Game::getLocalization()->Get("new_load_msg_" + String(progress.currentStage)).CString());
}

void Main::changeState(GameState newState) {
	gameState = newState;
	hud->updateStateVisibilty(newState);
}

void Main::HandleKeyUp(StringHash /*eventType*/, VariantMap& eventData) {
	int key = eventData[KeyUp::P_KEY].GetInt();

	if (key == KEY_ESCAPE) {
		auto console = GetSubsystem<Console>();
		if (console->IsVisible()) {
			console->SetVisible(false);
		} else {
			engine_->Exit();
		}
	}
	if (key == KEY_KP_0) {
		Game::getPlayersMan()->changeActive(0);
	} else if (key == KEY_KP_1) {
		Game::getPlayersMan()->changeActive(1);
	}
	if (gameState == GameState::RUNNING || gameState == GameState::PAUSE) {
		if (key == KEY_1) {
			changeCamera(CameraBehaviorType::FREE);
		} else if (key == KEY_2) {
			changeCamera(CameraBehaviorType::RTS);
		} else if (key == KEY_3) {
			changeCamera(CameraBehaviorType::TOP);
		} else if (key == KEY_F5) {
			String name = "test" + String(RandGen::nextRand(RandIntType::SAVE,99999999));
			save(name);
		} else if (key == KEY_F6) {
			saveToLoad = "quicksave.db";
			changeState(GameState::CLOSING);
		}
	}
}

void Main::HandleNewGame(StringHash eventType, VariantMap& eventData) {
	const auto element = static_cast<UIElement*>(eventData[UIMouseClick::P_ELEMENT].GetVoidPtr());
	const auto form = static_cast<NewGameForm*>(element->GetVar("NewGameForm").GetVoidPtr());

	changeState(GameState::NEW_GAME);
	newGameForm = new NewGameForm(*form);
}

void Main::HandleLoadGame(StringHash eventType, VariantMap& eventData) {
	const auto element = static_cast<UIElement*>(eventData[UIMouseClick::P_ELEMENT].GetVoidPtr());
	const auto fileName = element->GetVar("LoadFileName").GetString();

	changeState(GameState::LOADING);

	saveToLoad = String(fileName);
}

void Main::HandleCloseGame(StringHash eventType, VariantMap& eventData) {
	engine_->Exit();
}

void Main::HandleLeftMenuButton(StringHash eventType, VariantMap& eventData) {
	const auto hudData = HudData::getFromElement(eventData);

	switch (hudData->getType()) {
	case ActionType::BUILDING_CREATE:
		return controls->toBuild(hudData);
	default: ;
		controls->order(hudData->getId(), ActionParameter::Builder().setType(hudData->getType()).build());
	}
}

void Main::HandleSelectedButton(StringHash eventType, VariantMap& eventData) {
	controls->unSelectAll();
	const auto element = static_cast<UIElement*>(eventData[UIMouseClick::P_ELEMENT].GetVoidPtr());
	auto sHudElement = static_cast<SelectedHudElement*>(element->GetVar("SelectedHudElement").GetVoidPtr());
	controls->select(&sHudElement->getSelected());
}

void Main::HandleKeyDown(StringHash /*eventType*/, VariantMap& eventData) {
	const int key = eventData[KeyDown::P_KEY].GetInt();

	if (key == KEY_F1) {
		GetSubsystem<Console>()->Toggle();
	} else if (key == KEY_F2) {
		GetSubsystem<DebugHud>()->ToggleAll();
	}

	if (key == KEY_KP_PLUS) {
		simulation->changeCoef(coefToEdit, 1);
	} else if (key == KEY_KP_MINUS) {
		simulation->changeCoef(coefToEdit, -1);
	}

}

void Main::HandleMouseModeRequest(StringHash /*eventType*/, VariantMap& eventData) {
	const auto console = GetSubsystem<Console>();
	if (console && console->IsVisible()) { return; }
	auto input = GetSubsystem<Input>();
	if (useMouseMode_ == MM_ABSOLUTE) {
		input->SetMouseVisible(false);
	} else if (useMouseMode_ == MM_FREE) {
		input->SetMouseVisible(true);
	}
	input->SetMouseMode(useMouseMode_);
}

void Main::HandleMouseModeChange(StringHash /*eventType*/, VariantMap& eventData) {
	bool mouseLocked = eventData[MouseModeChanged::P_MOUSELOCKED].GetBool();
	GetSubsystem<Input>()->SetMouseVisible(!mouseLocked);
}

void Main::HandleSaveScene(StringHash /*eventType*/, VariantMap& eventData) {
	const auto element = static_cast<UIElement*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	const auto data = static_cast<FileFormData*>(element->GetVar("file_data").GetVoidPtr());
	save(data->fileName);
}

void Main::SetupViewport() const {
	SharedPtr<Viewport> viewport(new Viewport(context_, Game::getScene(),
	                                          Game::getCameraManager()->getComponent()));
	GetSubsystem<Renderer>()->SetViewport(0, viewport);
}

void Main::disposeScene() {
	if (inited) {
		loading loading2;
		Game::getScene()->SetUpdateEnabled(false);

		loading2.reset(5, "dispose simulation");
		delete simulation;
		simulation = nullptr;

		loading2.inc("dispose managers");
		delete Game::getCameraManager();
		Game::setCameraManager(nullptr);

		delete Game::getFormationManager();
		Game::setFormationManager(nullptr);

		delete Game::getColorPaletteRepo();
		Game::setColorPaletteRepo(nullptr);

		loading2.inc("dispose enviroment");
		delete Game::getEnvironment();
		Game::setEnvironment(nullptr);

		loading2.inc("dispose playerManager");
		delete Game::getPlayersMan();
		Game::setPlayersManager(nullptr);

		loading2.inc("dispose Stats & AIinput");
		delete Game::getAiInputProvider();
		Game::setAiInputProvider(nullptr);
		delete Game::getStats();
		Game::setStats(nullptr);

		loading2.inc("dispose controls");
		delete controls;
		controls = nullptr;

		loading2.inc("dispose levelBuilder");
		delete levelBuilder;
		levelBuilder = nullptr;
	}
	inited = false;
	loadingProgress.reset(loadStages);
	newGameProgress.reset(newGamesStages);
}

SelectedInfo* Main::control(const float timeStep, SimInfo* simulationInfo) {
	const IntVector2 cursorPos = Game::getUI()->GetCursorPosition();
	UIElement* element = Game::getUI()->GetElementAt(cursorPos, false);

	if (element) {
		controls->deactivate();
	} else {
		controls->activate();
		controls->control();
	}

	const auto input = GetSubsystem<Input>();
	debugManager.change(input, simulation);
	if (input->GetKeyPress(Urho3D::KEY_P)) {
		Game::getEnvironment()->drawInfluence();	
	}

	Game::getCameraManager()->translate(cursorPos, input, timeStep);
	Game::getCameraManager()->rotate(input->GetMouseMove());

	controls->cleanAndUpdate(simulationInfo);
	return controls->getInfo();
}
