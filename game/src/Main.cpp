#include "Main.h"

#include <numeric>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Console.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Engine/EngineDefs.h>
#include <Urho3D/Graphics/Camera.h>
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
#include <Urho3D/Graphics/RenderPath.h>
#include <Urho3D/UI/ProgressBar.h>

#include "camera/CameraEnums.h"
#include "camera/CameraManager.h"
#include "colors/ColorPaletteRepo.h"
#include "control/Controls.h"
#include "control/SelectedInfo.h"
#include "database/DatabaseCache.h"
#include "database/db_grah_structs.h"
#include "debug/DebugLineRepo.h"
#include "hud/Hud.h"
#include "hud/HudData.h"
#include "hud/MySprite.h"
#include "hud/window/in_game_menu/middle/FileFormData.h"
#include "hud/window/main_menu/new_game/NewGameForm.h"
#include "hud/window/selected/SelectedHudElement.h"
#include "math/RandGen.h"
#include "objects/ActionType.h"
#include "objects/projectile/ProjectileManager.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "scene/LevelBuilder.h"
#include "scene/load/dbload_container.h"
#include "simulation/FrameInfo.h"
#include "simulation/SimGlobals.h"
#include "simulation/SimInfo.h"
#include "simulation/Simulation.h"
#include "env/Environment.h"
#include "env/influence/CenterType.h"
#include "hud/UiUtils.h"
#include "simulation/formation/FormationManager.h"
#include "stats/AiInputProvider.h"


URHO3D_DEFINE_APPLICATION_MAIN(Main)

using namespace Urho3D;

Main::Main(Context* context) : Application(context), useMouseMode_(MM_ABSOLUTE), saver(100),
                               gameState(GameState::STARTING), loadingProgress(6) {
	if(!engineParameters_[EP_HEADLESS].GetBool()) {
		MySprite::RegisterObject(context);
	}

	Game::init();

	ProjectileManager::init();
}

void Main::Setup() {
	miniReadParameters();
	Game::setDatabaseCache(new DatabaseCache(SIM_GLOBALS.DATABASE_NUMBER.CString()));
	if (!SIM_GLOBALS.HEADLESS) {
		const db_settings* settings = Game::getDatabase()->getSettings();
		const db_graph_settings* graphSettings = Game::getDatabase()->getGraphSettings()[settings->graph];
		const db_resolution* resolution = Game::getDatabase()->getResolution(settings->resolution);
		engineParameters_[EP_WINDOW_TITLE] = GetTypeName();

		engineParameters_[EP_FULL_SCREEN] = graphSettings->fullscreen;

		engineParameters_[EP_SOUND] = false;
		engineParameters_[EP_WINDOW_WIDTH] = resolution->x;
		engineParameters_[EP_WINDOW_HEIGHT] = resolution->y;

		engine_->SetMaxFps(graphSettings->max_fps);
		engine_->SetMinFps(graphSettings->min_fps);
	}

	engineParameters_[EP_RESOURCE_PATHS] = "Data;CoreData;CoreDataMy";
	engineParameters_[EP_RESOURCE_PREFIX_PATHS] = " ;../";

	readParameters();
	RandGen::init(SIM_GLOBALS.RANDOM);
	if (SIM_GLOBALS.HEADLESS) {
		engineParameters_[EP_LOG_NAME] = "";
		GetSubsystem<Log>()->SetLevel(LOG_NONE);
	} else {
		engineParameters_[EP_LOG_NAME] = "logs/" + GetTypeName() + ".log";
	}
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
		healthBarProvider.init();
		subscribeToUIEvents();
	}

	InitMouseMode(MM_RELATIVE);
	changeState(GameState::LOADING);
}

void Main::writeOutput(std::initializer_list<const std::function<float(Player*)>> funcs1,
                       std::initializer_list<const std::function<std::span<float>(Player*)>> funcs2) const {
	std::ofstream outFile(("result/" + outputName).CString(), std::ios_base::out);
	for (const auto player : Game::getPlayersMan()->getAllPlayers()) {
		outFile << std::to_string(player->getId());
		for (auto& func : funcs1) {
			outFile << ";" << func(player);
		}

		for (auto& func : funcs2) {
			for (const auto val : func(player)) {
				outFile << ";" << val;
			}
		}
		outFile << "\n";
	}
	outFile.close();
}

void Main::writeOutput() const {
	if (!outputName.Empty()) {
		writeOutput(
			{
				[](Player* p) -> float { return p->getScore(); },
				[](Player* p) -> float { return p->getPossession().getUnitsNumber(); },
				[](Player* p) -> float { return p->getPossession().getBuildingsNumber(); }
			},
			{
				[](Player* p) -> std::span<float> { return p->getResources().getValues(); },
				[](Player* p) -> std::span<float> { return p->getResources().getSumValues(); },

				[](Player* p) -> std::span<float> { return p->getPossession().getUnitsMetrics(); },
				[](Player* p) -> std::span<float> { return p->getPossession().getBuildingsMetrics(); }
			});
	}
}

void Main::setCameraPos() const {
	if (!SIM_GLOBALS.HEADLESS) {
		auto camPos = Game::getEnvironment()
		              ->getCenterOf(CenterType::BUILDING, Game::getPlayersMan()->getActivePlayerID())
		              .value_or(Urho3D::Vector2::ZERO);
		Game::getCameraManager()->changePosition(camPos.x_, camPos.y_);
	}
}

void Main::Stop() {
	disposeScene();

	delete hud;
	delete Game::getDatabase();
	Game::setDatabaseCache(nullptr);
	Game::dispose();
	RandGen::dispose();
	ProjectileManager::dispose();

	if (!SIM_GLOBALS.HEADLESS) { engine_->DumpResources(true); }

	const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now() - SimGlobals::SUPER_START);

	std::cout << "ENDED at " << duration.count() << " ms" << std::endl;
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
		if (SimGlobals::CURRENT_RUN + 1 < SimGlobals::MAX_RUNS) {
			changeState(GameState::CLOSING);
		} else {
			engine_->Exit();
		}
	}
}

void Main::HandleUpdate(StringHash eventType, VariantMap& eventData) {
	switch (gameState) {
	case GameState::MENU_MAIN:
		//changeState(GameState::LOADING);
		break;
	case GameState::LOADING:
		load(saveToLoad, nullptr);
		break;
	case GameState::RUNNING:
		running(eventData[SceneUpdate::P_TIMESTEP].GetDouble());
		break;
	case GameState::PAUSE:
		break;
	case GameState::CLOSING:
		disposeScene();

		changeState(GameState::LOADING);
		break;
	case GameState::NEW_GAME:
		load(saveToLoad, newGameForm);
		break;
	case GameState::STARTING:
		break;
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
	const auto l10n = GetSubsystem<Localization>();
	if (!SIM_GLOBALS.HEADLESS) {
		l10n->LoadJSONFile("lang/language.json");
	}

	Game::setLocalization(l10n);
}

void Main::save(const String& name) {
	saver.createSave(name);
	int mapId = 1; //TODO id mapy wpisac
	saver.saveConfig(mapId, Game::getEnvironment()->getResolution());
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
		->setColorPaletteRepo(new ColorPaletteRepo());
}

void Main::updateProgress(Loading& progress) const {
	if (!SIM_GLOBALS.HEADLESS) {
		std::string msg = Game::getLocalization()->Get("load_msg_" +
			String((int)loadingProgress.currentStage)).CString();
		progress.inc(std::move(msg));
		hud->updateLoading(progress.getProgress());
	} else {
		progress.inc();
	}
}

void Main::load(const String& saveName, NewGameForm* form) {
	switch (loadingProgress.currentStage) {
	case 0: {
		RandGen::reset(SIM_GLOBALS.RANDOM);
		ProjectileManager::reset();
		//disposeScene();
		Game::getDatabase()->refreshAfterParametersRead();
		setSimpleManagers();

		levelBuilder = new LevelBuilder();

		if (form) {
			Game::getPlayersMan()->load(form);
		} else {
			loader.createLoad(saveName, SIM_GLOBALS.CURRENT_RUN > 0);
			Game::getPlayersMan()->load(loader.loadPlayers(), loader.loadResources());
		}

		if (!engineParameters_[EP_HEADLESS].GetBool()) {
			SetupViewport();
			controls = new Controls(GetSubsystem<Input>());
		}
		hud->resetLoading();
		if (form) {
			levelBuilder->createScene(form);
		} else {
			levelBuilder->createScene(loader);
		}
	}
	break;
	case 1: {
		if (form) {
			createEnv(form->size);
		} else {
			createEnv(loader.getConfig()->size);
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
		if (form) {
			simulation->initScene(form);
		} else {
			simulation->initScene(loader);
		}

		simulation->forceUpdateInfluenceMaps();
		setCameraPos();
		break;
	case 5:
		delete form; //TODO trzeba ustawic na null
		loader.end();
		changeState(GameState::RUNNING);
		inited = true;
		break;
	}
	updateProgress(loadingProgress);
}

void Main::createEnv(unsigned short mainMapResolution) const {
	Game::setEnvironment(new Environment(levelBuilder->getTerrain(), mainMapResolution));
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
		} else if (key == KEY_H) {
			auto& possession = Game::getPlayersMan()->getActivePlayer()->getPossession();
			for (auto building : possession.getBuildings()) {
				//TODO perf
				if (building->getDbBuilding()->typeCenter) {
					auto pos = building->getPosition();
					Game::getCameraManager()->changePosition(pos.x_, pos.z_);
				}
			}
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
	if (key == KEY_G) {
		Game::getEnvironment()->flipTerrainShaderParam("GridEnable");
	}
	if (key == KEY_V) {
		Game::getEnvironment()->nextVisibilityType();
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
	auto v = new Viewport(context_, Game::getScene(), Game::getCameraManager()->getComponent());
	SharedPtr<Viewport> viewport(v);

	//v->GetRenderPath()->Append(Game::getCache()->GetResource<XMLFile>("PostProcess/FXAA2.xml"));

	const auto renderer = GetSubsystem<Renderer>();
	if (renderer) {
		renderer->SetViewport(0, viewport);
	}
}

void Main::disposeScene() {
	if (inited) {
		writeOutput();
		Loading loading2(5, !SIM_GLOBALS.HEADLESS);
		Game::getScene()->SetUpdateEnabled(false);

		loading2.reset("dispose simulation");
		if (simulation) {
			simulation->clearNodesWithoutDelete();
			delete simulation;
			simulation = nullptr;
		}

		loading2.inc("dispose managers");
		delete Game::getCameraManager();
		Game::setCameraManager(nullptr);

		delete Game::getFormationManager();
		Game::setFormationManager(nullptr);

		delete Game::getColorPaletteRepo();
		Game::setColorPaletteRepo(nullptr);

		loading2.inc("dispose environment");
		delete Game::getEnvironment();
		Game::setEnvironment(nullptr);

		loading2.inc("dispose playerManager");
		delete Game::getPlayersMan();
		Game::setPlayersManager(nullptr);

		loading2.inc("dispose Stats & AIinput");
		delete Game::getAiInputProvider();
		Game::setAiInputProvider(nullptr);

		loading2.inc("dispose controls");
		delete controls;
		controls = nullptr;

		loading2.inc("dispose levelBuilder");
		delete levelBuilder;
		levelBuilder = nullptr;
		DebugLineRepo::dispose();
	}
	SimGlobals::CURRENT_RUN++;
	if (SimGlobals::CURRENT_RUN < SimGlobals::OUTPUT_NAMES.Size()) {
		outputName = SimGlobals::OUTPUT_NAMES[SimGlobals::CURRENT_RUN];
	}

	inited = false;
	loadingProgress.reset();
}

SelectedInfo* Main::control(const float timeStep, SimInfo* simulationInfo) {
	const IntVector2 cursorPos = Game::getUI()->GetCursorPosition();
	UIElement* element = Game::getUI()->GetElementAt(hud->getRoot(), cursorPos, true);

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
	auto camera = Game::getCameraManager()->getComponent();
	healthBarProvider.reset(controls->getSelected().size(), controls->getInfo()->getSelectedType());
	for (auto selected : controls->getSelected()) {
		auto progressBar = healthBarProvider.getNext(selected->getType());

		if (progressBar) {
			auto pos = selected->getPosition();
			pos.y_ += selected->getModelHeight() * 1.3f;
			IntVector2 pixel{
				VectorRoundToInt(Vector2(Game::getGraphics()->GetSize()) * camera->WorldToScreenPoint(pos))};

			auto halfSize = progressBar->GetSize().x_ / 2;
			pixel.x_ -= halfSize;
			progressBar->SetPosition(pixel);
			progressBar->SetValue(selected->getHealthPercent());
			progressBar->SetVisible(true);
			progressBar->SetEnabled(true);
		}
	}

	return controls->getInfo();
}

void Main::readParameters() {
	auto const& arguments = GetArguments();

	for (unsigned i = 0; i < arguments.Size(); ++i) {
		auto& current = arguments[i];
		if (current.Length() > 1 && current[0] == '-') {
			String argument = current.Substring(1).ToLower();
			const String value = i + 1 < arguments.Size() ? arguments[i + 1] : String::EMPTY;

			if (argument == "train") {
				SimGlobals::TRAIN_MODE = true;
				engine_->SetMaxFps(0);
				engine_->SetMaxInactiveFps(0);
			} else if (argument == "benchmark") {
				SimGlobals::BENCHMARK_MODE = true;
				engine_->SetMaxFps(0);
				engine_->SetMaxInactiveFps(0);
			} else if (argument == "headless") {
				SimGlobals::HEADLESS = true;
			} else if (argument == "faketerrain") {
				SimGlobals::FAKE_TERRAIN = true;
			} else if (argument == "allplayerai") {
				SimGlobals::ALL_PLAYER_AI = true;
			} else if (argument == "noplayerai") {
				SimGlobals::NO_PLAYER_AI = true;
			} else if (argument == "savename") {
				saveToLoad = SIM_GLOBALS.DATABASE_NUMBER + value;
			} else if (argument == "outputname" && !value.Empty()) {
				SimGlobals::OUTPUT_NAMES = value.Split('|');
				SimGlobals::MAX_RUNS = SimGlobals::OUTPUT_NAMES.Size();
				outputName = SimGlobals::OUTPUT_NAMES[0];
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
				SimGlobals::ACTION_AI_PATH[0] = value.CString();
				++i;
			} else if (argument == "actionaipath2" && !value.Empty()) {
				SimGlobals::ACTION_AI_PATH[1] = value.CString();
				++i;
			} else if (argument == "orderaipath1" && !value.Empty()) {
				SimGlobals::ORDER_AI_PATH[0] = value.CString();
				++i;
			} else if (argument == "orderaipath2" && !value.Empty()) {
				SimGlobals::ORDER_AI_PATH[1] = value.CString();
				++i;
			} else if (argument == "random") {
				SimGlobals::RANDOM = true;
			}
		}
	}
}

void Main::miniReadParameters() const {
	auto const& arguments = GetArguments();

	for (unsigned i = 0; i < arguments.Size(); ++i) {
		if (arguments[i].Length() > 1 && arguments[i][0] == '-') {
			String argument = arguments[i].Substring(1).ToLower();
			const String value = i + 1 < arguments.Size() ? arguments[i + 1] : String::EMPTY;

			if (argument == "databasename" && !value.Empty()) {
				SimGlobals::DATABASE_NUMBER = value;
				++i;
			} else if (argument == "headless") {
				SimGlobals::HEADLESS = true;
			}
		}
	}

}
