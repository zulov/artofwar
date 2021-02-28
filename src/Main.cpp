#include "Main.h"

#include <numeric>
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
#include "math/SpanUtils.h"
#include "objects/ActionType.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "scene/LevelBuilder.h"
#include "simulation/FrameInfo.h"
#include "simulation/SimGlobals.h"
#include "simulation/SimInfo.h"
#include "simulation/Simulation.h"
#include "simulation/env/Environment.h"
#include "simulation/formation/FormationManager.h"
#include "stats/AiInputProvider.h"
#include "stats/Stats.h"


URHO3D_DEFINE_APPLICATION_MAIN(Main)

using namespace Urho3D;

Main::Main(Context* context) : Application(context), useMouseMode_(MM_ABSOLUTE), saver(100),
                               gameState(GameState::STARTING), loadingProgress(6),
                               newGameProgress(6) {
	MySprite::RegisterObject(context);
	Game::init();
	RandGen::init();
}

void Main::Setup() {
	miniReadParameters();
	Game::setDatabaseCache(new DatabaseCache(SIM_GLOBALS.DATABASE_NUMBER.CString()));
	db_settings* settings = Game::getDatabase()->getSettings();
	db_graph_settings* graphSettings = Game::getDatabase()->getGraphSettings()[settings->graph];
	db_resolution* resolution = Game::getDatabase()->getResolution(settings->resolution);
	engineParameters_[EP_WINDOW_TITLE] = GetTypeName();

	engineParameters_[EP_FULL_SCREEN] = graphSettings->fullscreen;

	engineParameters_[EP_SOUND] = false;
	engineParameters_[EP_WINDOW_WIDTH] = resolution->x;
	engineParameters_[EP_WINDOW_HEIGHT] = resolution->y;
	engineParameters_[EP_RESOURCE_PATHS] = "Data;CoreData;CoreDataMy";
	engineParameters_[EP_RESOURCE_PREFIX_PATHS] = " ;../";

	engine_->SetMaxFps(graphSettings->max_fps);
	engine_->SetMinFps(graphSettings->min_fps);

	readParameters();
	if (SIM_GLOBALS.HEADLESS) {
		engineParameters_[EP_LOG_NAME] = "";
		GetSubsystem<Log>()->SetLevel(LOG_NONE);
	} else {
		engineParameters_[EP_LOG_NAME] = "logs/" + GetTypeName() + ".log";
	}

	Game::getDatabase()->refreshAfterParametersRead();

	Game::setCache(GetSubsystem<ResourceCache>())
		->setUI(GetSubsystem<UI>())
		->setConsole(GetSubsystem<Console>())
		->setContext(context_)
		->setLog(GetSubsystem<Log>());
}

void Main::Start() {
	Game::setGraphics(GetSubsystem<Graphics>());

	SetWindowTitleAndIcon();
	InitLocalizationSystem();

	subscribeToEvents();

	hud = new Hud();
	if (!engineParameters_[EP_HEADLESS].GetBool()) {
		hud->prepareUrho(engine_);
		hud->createMyPanels();
		subscribeToUIEvents();
	}

	InitMouseMode(MM_RELATIVE);
	changeState(GameState::LOADING);
}

void Main::writeOutput(const std::function<float(Player*)>& func) const {
	std::ofstream outFile(("result/" + outputName).CString(), std::ios_base::out);
	for (auto player : Game::getPlayersMan()->getAllPlayers()) {
		outFile << std::to_string(player->getId()) << ";" << func(player) << "\n";
	}
	outFile.close();
}

void Main::writeOutput() const {
	if (!outputType.Empty()) {
		std::ofstream outFile(("result/" + outputName).CString(), std::ios_base::out);
		if (outputType == "score") {
			writeOutput([](Player* p) -> float { return p->getScore(); });
		} else if (outputType == "ressum") {
			writeOutput([](Player* p) -> float { return sumSpan(p->getResources().getValues()); });
		} else if (outputType == "ressmallest") {
			writeOutput([](Player* p) -> float { return minSpan(p->getResources().getValues()); });
		} else if (outputType == "ressmallest+sum") {
			writeOutput([](Player* p) -> float { return minAndSumSpan(p->getResources().getValues()); });
		} else if (outputType == "res_max_min") {
			writeOutput([](Player* p) -> float {
				return minSpanSq(p->getResources().getValues()) + maxSpanRoot(p->getResources().getValues());
			});
		} else if (outputType == "armysum") {
			writeOutput([](Player* p) -> float { return sumSpan(p->getPossession().getUnitsMetrics()); });
		} else if (outputType == "res_sum_min_max") {
			writeOutput([](Player* p) -> float {
				return minSpan(p->getResources().getSumValues()) + maxSpanRoot(p->getResources().getSumValues());
			});
		} else if (outputType == "defence") {
			writeOutput([](Player* p) -> float {
				return p->getPossession().getBuildingsVal(BuildingMetric::DEFENCE)
					+ p->getPossession().getUnitsVal(UnitMetric::DEFENCE);
			});
		} else if (outputType == "attack") {
			writeOutput([](Player* p) -> float {
				auto& poss = p->getPossession();
				return poss.getBuildingsVal(BuildingMetric::DISTANCE_ATTACK)
					+ poss.getUnitsVal(UnitMetric::DISTANCE_ATTACK)
					+ poss.getUnitsVal(UnitMetric::BUILDING_ATTACK)
					+ poss.getUnitsVal(UnitMetric::CHARGE_ATTACK)
					+ poss.getUnitsVal(UnitMetric::CLOSE_ATTACK);
			});
		}
	}
}

void Main::Stop() {
	writeOutput();

	disposeScene();

	delete hud;
	delete Game::getDatabase();
	Game::setDatabaseCache(nullptr);
	Game::dispose();
	RandGen::dispose();
	if (!SIM_GLOBALS.HEADLESS) {
		engine_->DumpResources(true);
	}
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


	SimInfo* simInfo = simulation->update(timeStep);
	if (!engineParameters_[EP_HEADLESS].GetBool()) {
		benchmark.add(1.0f / timeStep);
		debugManager.draw();
		SelectedInfo* selectedInfo = control(timeStep, simInfo);
		hud->update(benchmark, Game::getCameraManager(), selectedInfo, simInfo);
	}

	if (timeLimit != -1 && simInfo->getFrameInfo()->getSeconds() > timeLimit) {
		engine_->Exit();
	}
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
	if (graphics) {
		const auto icon = Game::getCache()->GetResource<Image>("textures/UrhoIcon.png");
		graphics->SetWindowIcon(icon);
		graphics->SetWindowTitle("Art of War 2017");
	}
}

void Main::changeCamera(CameraBehaviorType type) {
	if (!engineParameters_[EP_HEADLESS].GetBool()) {
		Game::getCameraManager()->setCameraBehave(type);
		SetupViewport();
		InitMouseMode(Game::getCameraManager()->getMouseMode());
	}
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

void Main::updateProgress(Loading& progress, std::string msg) const {
	progress.inc(std::move(msg));
	hud->updateLoading(progress.getProgress());
}

void Main::load(const String& saveName, Loading& progress) {
	switch (progress.currentStage) {
	case 0: {
		setSimpleManagers();

		loader.createLoad(saveName);
		levelBuilder = new LevelBuilder();
		controls = new Controls(GetSubsystem<Input>());
		SetupViewport();

		Game::getPlayersMan()->load(loader.loadPlayers(), loader.loadResources());
		Game::getStats()->init();

		if (!engineParameters_[EP_HEADLESS].GetBool()) {
			controls->init();
			subscribeToUIEvents();
		}
		hud->resetLoading();
		levelBuilder->createScene(loader);
	}
	break;
	case 1: {
		createEnv();
		if (SIM_GLOBALS.CAMERA_START != Urho3D::Vector2::ZERO) {
			Game::getCameraManager()->changePosition(SIM_GLOBALS.CAMERA_START.x_, SIM_GLOBALS.CAMERA_START.y_);
		}
		break;
	}
	case 2: {
		Game::getEnvironment()->prepareGridToFind();
		hud->createMiniMap();
		break;
	}
	case 3:
		createSimulation();
		break;
	case 4:
		simulation->initScene(loader);
		simulation->forceUpdateInfluenceMaps();
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

void Main::newGame(NewGameForm* form, Loading& progress) {
	switch (progress.currentStage) {
	case 0: {
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
	case 1: {
		createEnv();
		break;
	}
	case 2: {
		Game::getEnvironment()->prepareGridToFind();
		hud->createMiniMap();
		break;
	}
	case 3:
		createSimulation();
		break;
	case 4:
		simulation->initScene(form);
		simulation->forceUpdateInfluenceMaps();
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
			String name = "test" + String(RandGen::nextRand(RandIntType::SAVE, 99999999));
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
		controls->order(hudData->getId(), hudData->getType());
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

	if (key == KEY_F8) {
		engine_->SetMaxFps((engine_->GetMaxFps() + 1) * 2);
	} else if (key == KEY_F7) {
		engine_->SetMaxFps(engine_->GetMaxFps() / 2);
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

void Main::SetupViewport() {
	if (!engineParameters_[EP_HEADLESS].GetBool()) {
		SharedPtr<Viewport> viewport(new Viewport(context_, Game::getScene(),
		                                          Game::getCameraManager()->getComponent()));
		const auto renderer = GetSubsystem<Renderer>();
		if (renderer) {
			GetSubsystem<Renderer>()->SetViewport(0, viewport);
		}
	}
}

void Main::disposeScene() {
	if (inited) {
		Loading loading2(5);
		Game::getScene()->SetUpdateEnabled(false);

		loading2.reset("dispose simulation");
		simulation->clearNodesWithoutDelete();
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
	loadingProgress.reset();
	newGameProgress.reset();
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

	controls->cleanAndUpdate(simulationInfo);
	return controls->getInfo();
}

void Main::readParameters() {
	auto arguments = GetArguments();

	for (unsigned i = 0; i < arguments.Size(); ++i) {
		if (arguments[i].Length() > 1 && arguments[i][0] == '-') {
			String argument = arguments[i].Substring(1).ToLower();
			const String value = i + 1 < arguments.Size() ? arguments[i + 1] : String::EMPTY;

			if (argument == "trainmode") {
				SimGlobals::TRAIN_MODE = true;
				engine_->SetMaxFps(0);
				engine_->SetMaxInactiveFps(0);
			} else if (argument == "benchmarkmode") {
				SimGlobals::BENCHMARK_MODE = true;
				engine_->SetMaxFps(0);
				engine_->SetMaxInactiveFps(0);
			} else if (argument == "headless") {
				SimGlobals::HEADLESS = true;
			} else if (argument == "savename") {
				saveToLoad = value;
			} else if (argument == "outputtype" && !value.Empty()) {
				outputType = value;
				++i;
			} else if (argument == "outputname" && !value.Empty()) {
				outputName = value;
				++i;
			} else if (argument == "timelimit" && !value.Empty()) {
				timeLimit = ToInt(value);
				++i;
			} else if (argument == "x" && !value.Empty()) {
				engineParameters_[EP_WINDOW_WIDTH] = ToInt(value);
				++i;
			} else if (argument == "y" && !value.Empty()) {
				engineParameters_[EP_WINDOW_HEIGHT] = ToInt(value);
				++i;
			} else if (argument == "actionaipath1" && !value.Empty()) {
				SimGlobals::ACTION_AI_PATH[0] = value;
				++i;
			} else if (argument == "actionaipath2" && !value.Empty()) {
				SimGlobals::ACTION_AI_PATH[1] = value;
				++i;
			} else if (argument == "orderaipath1" && !value.Empty()) {
				SimGlobals::ORDER_AI_PATH[0] = value;
				++i;
			} else if (argument == "orderaipath2" && !value.Empty()) {
				SimGlobals::ORDER_AI_PATH[1] = value;
				++i;
			} else if (argument == "camposx" && !value.Empty()) {
				SimGlobals::CAMERA_START.x_ = ToInt(value);
				++i;
			} else if (argument == "camposy" && !value.Empty()) {
				SimGlobals::CAMERA_START.y_ = ToInt(value);
				++i;
			}
		}
	}
}

void Main::miniReadParameters() const {
	auto arguments = GetArguments();

	for (unsigned i = 0; i < arguments.Size(); ++i) {
		if (arguments[i].Length() > 1 && arguments[i][0] == '-') {
			String argument = arguments[i].Substring(1).ToLower();
			const String value = i + 1 < arguments.Size() ? arguments[i + 1] : String::EMPTY;

			if (argument == "databasename" && !value.Empty()) {
				SimGlobals::DATABASE_NUMBER = value;
				++i;
			}
		}
	}
}
