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
#include "simulation/Simulation.h"
#include "env/Environment.h"
#include "env/influence/CenterType.h"
#include "hud/UiUtils.h"
#include "player/Possession.h"
#include "simulation/formation/FormationManager.h"
#include "stats/AiInputProvider.h"
#include "utils/DebugUtils.h"

URHO3D_DEFINE_APPLICATION_MAIN(Main)


Main::Main(Urho3D::Context* context) : Application(context), useMouseMode_(Urho3D::MM_ABSOLUTE), saver(100),
                                       gameState(GameState::STARTING), loadingProgress(4) {
	if (!engineParameters_[Urho3D::EP_HEADLESS].GetBool()) {
		MySprite::RegisterObject(context);
	}

	Game::init();

	ProjectileManager::init();
}

void Main::Setup() {
	miniReadParameters();
	const std::string prefix = std::string(SIM_GLOBALS.DATABASE_NUMBER.CString()) + ".db";
	Game::setDatabaseCache(new DatabaseCache(prefix));
	if (!SIM_GLOBALS.HEADLESS) {
		const db_settings* settings = Game::getDatabase()->getSettings();
		const db_graph_settings* graphSettings = Game::getDatabase()->getGraphSettings()[settings->graph];
		const db_resolution* resolution = Game::getDatabase()->getResolution(settings->resolution);
		engineParameters_[Urho3D::EP_WINDOW_TITLE] = GetTypeName();

		engineParameters_[Urho3D::EP_FULL_SCREEN] = graphSettings->fullscreen;

		engineParameters_[Urho3D::EP_SOUND] = false;
		engineParameters_[Urho3D::EP_WINDOW_WIDTH] = resolution->x;
		engineParameters_[Urho3D::EP_WINDOW_HEIGHT] = resolution->y;

		engine_->SetMaxFps(graphSettings->max_fps);
		engine_->SetMinFps(graphSettings->min_fps);
	}
	engineParameters_[Urho3D::EP_RESOURCE_PREFIX_PATHS] = " ;../";
	if (!SIM_GLOBALS.HEADLESS || !SIM_GLOBALS.FAKE_TERRAIN) {
		engineParameters_[Urho3D::EP_RESOURCE_PATHS] = "Data;CoreData;CoreDataMy";
	} else {
		engineParameters_[Urho3D::EP_RESOURCE_PATHS] = "";
		engineParameters_[Urho3D::EP_AUTOLOAD_PATHS] = "";
	}

	readParameters();
	RandGen::init(SIM_GLOBALS.RANDOM);
	if (SIM_GLOBALS.HEADLESS) {
		engineParameters_[Urho3D::EP_LOG_NAME] = "";
		GetSubsystem<Urho3D::Log>()->SetLevel(Urho3D::LOG_NONE);
	} else {
		engineParameters_[Urho3D::EP_LOG_NAME] = "logs/" + GetTypeName() + ".log";
	}
	if (!SIM_GLOBALS.HEADLESS) {
		Game::setConsole(GetSubsystem<Urho3D::Console>())
			->setLog(GetSubsystem<Urho3D::Log>());
	}
	if (!SIM_GLOBALS.HEADLESS || !SIM_GLOBALS.FAKE_TERRAIN) {
		Game::setCache(GetSubsystem<Urho3D::ResourceCache>());
	}
	Game::setContext(context_);
}

void Main::Start() {
	SubscribeToEvent(Urho3D::E_UPDATE, URHO3D_HANDLER(Main, HandleUpdate));
	if (!engineParameters_[Urho3D::EP_HEADLESS].GetBool()) {
		Game::setGraphics(GetSubsystem<Urho3D::Graphics>());

		SetWindowTitleAndIcon();
		InitLocalizationSystem();
		subscribeToEvents();
		Game::setUI(GetSubsystem<Urho3D::UI>());
		hud = new Hud();

		hud->prepareUrho(engine_);
		hud->createMyPanels();
		healthBarProvider.init();
		subscribeToUIEvents();
		InitMouseMode(Urho3D::MM_RELATIVE);
	}

	changeState(GameState::LOADING);
}

void Main::writeOutput(std::initializer_list<const std::function<float(Player*)>> funcs1,
                       std::initializer_list<const std::function<std::span<const float>(Player*)>> funcs2) const {
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
			            [](Player* p) -> float { return p->getPossession()->getUnitsNumber(); },
			            [](Player* p) -> float { return p->getPossession()->getBuildingsNumber(); }
		            },
		            {
			            [](Player* p) -> std::span<float> { return p->getResources()->getValues(); },
			            [](Player* p) -> std::span<float> { return p->getResources()->getSumValues(); },

			            [](Player* p) -> std::span<float> { return p->getPossession()->getUnitsMetrics(); },
			            [](Player* p) -> std::span<float> { return p->getPossession()->getBuildingsMetrics(); }
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
	                                                                            std::chrono::system_clock::now() -
	                                                                            SimGlobals::SUPER_START);
	PRINT_X2Y();
	PRINT_COUNTERS();
	std::cout << "ENDED at " << duration.count() << " ms" << std::endl;
}

void Main::subscribeToUIEvents() {
	for (auto hudData : hud->getButtonsLeftMenuToSubscribe()) {
		SubscribeToEvent(hudData->getUIParent(), Urho3D::E_CLICK, URHO3D_HANDLER(Main, HandleLeftMenuButton));
	}

	for (auto buttton : hud->getButtonsSelectedToSubscribe()) {
		SubscribeToEvent(buttton, Urho3D::E_CLICK, URHO3D_HANDLER(Main, HandleSelectedButton));
	}

	SubscribeToEvent(hud->getSaveButton(), Urho3D::E_CLICK, URHO3D_HANDLER(Main, HandleSaveScene));
	SubscribeToEvent(hud->getNewGameProceed(), Urho3D::E_CLICK, URHO3D_HANDLER(Main, HandleNewGame));
	SubscribeToEvent(hud->getLoadButton(), Urho3D::E_CLICK, URHO3D_HANDLER(Main, HandleLoadGame));
	SubscribeToEvent(hud->getCloseButton(), Urho3D::E_CLICK, URHO3D_HANDLER(Main, HandleCloseGame));

	hud->subscribeToUIEvents();
}

void Main::subscribeToEvents() {
	SubscribeToEvent(Urho3D::E_KEYDOWN, URHO3D_HANDLER(Main, HandleKeyDown));
	SubscribeToEvent(Urho3D::E_KEYUP, URHO3D_HANDLER(Main, HandleKeyUp));
}

void Main::running(const float timeStep) {
	FrameInfo* frameInfo;
	for (int i = 0; i < SIM_GLOBALS.FRAME_PACK; ++i) {
		frameInfo = simulation->update(timeStep);

		if (!SIM_GLOBALS.HEADLESS) {
			Game::addTime(timeStep);
			benchmark.add(1.0f / timeStep);
			debugManager.draw();
			SelectedInfo* selectedInfo = control(timeStep, frameInfo);
			hud->update(benchmark, Game::getCameraManager(), selectedInfo, frameInfo);
		}
	}

	if (timeLimit != -1 && frameInfo->getSeconds() >= timeLimit) {
		if (SimGlobals::CURRENT_RUN + 1 < SimGlobals::MAX_RUNS) {
			changeState(GameState::CLOSING);
		} else {
			engine_->Exit();
		}
	}
}

void Main::HandleUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData) {
	switch (gameState) {
	case GameState::MENU_MAIN:
		//changeState(GameState::LOADING);
		break;
	case GameState::LOADING:
		load(saveToLoad, nullptr);
		break;
	case GameState::RUNNING:
		running(eventData[Urho3D::SceneUpdate::P_TIMESTEP].GetFloat());
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

void Main::InitMouseMode(Urho3D::MouseMode mode) {
	useMouseMode_ = mode;
	auto input = GetSubsystem<Urho3D::Input>();

	Urho3D::Console* console = Game::getConsole();
	if (useMouseMode_ != Urho3D::MM_ABSOLUTE) {
		input->SetMouseMode(useMouseMode_);
		if (console && console->IsVisible()) {
			input->SetMouseMode(Urho3D::MM_ABSOLUTE, true);
		}
	}
}

void Main::SetWindowTitleAndIcon() {
	Urho3D::Graphics* graphics = Game::getGraphics();
	if (graphics) {
		const auto icon = Game::getCache()->GetResource<Urho3D::Image>("textures/UrhoIcon.png");
		graphics->SetWindowIcon(icon);
		graphics->SetWindowTitle("Art of War 2017");
	}
}

void Main::changeCamera(CameraBehaviorType type) {
	if (!engineParameters_[Urho3D::EP_HEADLESS].GetBool()) {
		Game::getCameraManager()->setCameraBehave(type);
		SetupViewport();
		InitMouseMode(Game::getCameraManager()->getMouseMode());
	}
}

void Main::InitLocalizationSystem() const {
	const auto l10n = GetSubsystem<Urho3D::Localization>();
	if (!SIM_GLOBALS.HEADLESS) {
		
		l10n->LoadJSONFile("lang/language.json");
		l10n->LoadJSONFile("lang/top_language.json");
	}

	Game::setLocalization(l10n);
}

void Main::save(const Urho3D::String& name) {
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
		                                               Urho3D::String((int)loadingProgress.currentStage)).CString();
		progress.inc(std::move(msg));
		hud->updateLoading(progress.getProgress());
	} else {
		progress.inc();
	}
}

void Main::load(const Urho3D::String& saveName, NewGameForm* form) {
	for (int i = 0; i < std::min(SIM_GLOBALS.FRAME_PACK, loadingProgress.stagesNumber); ++i) {
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

			if (!engineParameters_[Urho3D::EP_HEADLESS].GetBool()) {
				SetupViewport();
				controls = new Controls(GetSubsystem<Urho3D::Input>());
			}
			if (hud) {
				hud->resetLoading();
			}

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
			Game::getEnvironment()->prepareGridToFind();
			if (hud) {
				hud->createMiniMap();
			}

			break;
		}
		case 2:
			createSimulation();
			if (form) {
				simulation->initScene(form);
			} else {
				simulation->initScene(loader);
			}

			simulation->updateInfluenceMaps(true);
			setCameraPos();
			break;
		case 3:
			delete form; //TODO trzeba ustawic na null
			loader.end();
			changeState(GameState::RUNNING);
			inited = true;
			break;
		}
		updateProgress(loadingProgress);
	}
}

void Main::createEnv(unsigned short mainMapResolution) const {
	Game::setEnvironment(new Environment(levelBuilder->getTerrain(), mainMapResolution));
}

void Main::changeState(GameState newState) {
	gameState = newState;
	if (hud) {
		hud->updateStateVisibilty(newState);
	}
}

void Main::HandleKeyUp(Urho3D::StringHash /*eventType*/, Urho3D::VariantMap& eventData) {
	int key = eventData[Urho3D::KeyUp::P_KEY].GetInt();

	if (key == Urho3D::KEY_ESCAPE) {
		auto console = GetSubsystem<Urho3D::Console>();
		if (console->IsVisible()) {
			console->SetVisible(false);
		} else {
			engine_->Exit();
		}
	}
	if (key == Urho3D::KEY_KP_0) {
		Game::getPlayersMan()->changeActive(0);
	} else if (key == Urho3D::KEY_KP_1) {
		Game::getPlayersMan()->changeActive(1);
	}
	if (gameState == GameState::RUNNING || gameState == GameState::PAUSE) {
		if (key == Urho3D::KEY_1) {
			changeCamera(CameraBehaviorType::FREE);
		} else if (key == Urho3D::KEY_2) {
			changeCamera(CameraBehaviorType::RTS);
		} else if (key == Urho3D::KEY_3) {
			changeCamera(CameraBehaviorType::TOP);
		} else if (key == Urho3D::KEY_F5) {
			Urho3D::String name = "test" + Urho3D::String(RandGen::nextRand(RandIntType::SAVE, 99999999));
			save(name);
		} else if (key == Urho3D::KEY_F6) {
			saveToLoad = "quicksave.db";
			changeState(GameState::CLOSING);
		} else if (key == Urho3D::KEY_H) {
			const auto possession = Game::getPlayersMan()->getActivePlayer()->getPossession();
			for (const auto building : possession->getBuildings()) {
				if (building->getDb()->typeCenter) {
					auto pos = building->getPosition();
					Game::getCameraManager()->changePosition(pos.x_, pos.z_);
				}
			}
		}
	}
}

void Main::HandleNewGame(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData) {
	const auto element = static_cast<Urho3D::UIElement*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	const auto form = static_cast<NewGameForm*>(element->GetVar("NewGameForm").GetVoidPtr());

	changeState(GameState::NEW_GAME);
	newGameForm = new NewGameForm(*form);
}

void Main::HandleLoadGame(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData) {
	const auto element = static_cast<Urho3D::UIElement*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	const auto fileName = element->GetVar("LoadFileName").GetString();

	changeState(GameState::LOADING);

	saveToLoad = Urho3D::String(fileName);
}

void Main::HandleCloseGame(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData) {
	engine_->Exit();
}

void Main::HandleLeftMenuButton(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData) {
	const auto hudData = HudData::getFromElement(eventData);

	switch (hudData->getType()) {
	case ActionType::BUILDING_CREATE:
		return controls->toBuild(hudData);
	default: ;
		controls->order(hudData->getId(), hudData->getType());
	}
}

void Main::HandleSelectedButton(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData) {
	controls->unSelectAll();
	const auto element = static_cast<Urho3D::UIElement*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	auto sHudElement = static_cast<SelectedHudElement*>(element->GetVar("SelectedHudElement").GetVoidPtr());
	controls->select(&sHudElement->getSelected());
}

void Main::HandleKeyDown(Urho3D::StringHash /*eventType*/, Urho3D::VariantMap& eventData) {
	const int key = eventData[Urho3D::KeyDown::P_KEY].GetInt();

	if (key == Urho3D::KEY_F1) {
		GetSubsystem<Urho3D::Console>()->Toggle();
	} else if (key == Urho3D::KEY_F2) {
		GetSubsystem<Urho3D::DebugHud>()->ToggleAll();
	}

	if (key == Urho3D::KEY_KP_PLUS) {
		simulation->changeCoef(coefToEdit, 1);
	} else if (key == Urho3D::KEY_KP_MINUS) {
		simulation->changeCoef(coefToEdit, -1);
	}

	if (key == Urho3D::KEY_F8) {
		engine_->SetMaxFps((engine_->GetMaxFps() + 1) * 2);
	} else if (key == Urho3D::KEY_F7) {
		engine_->SetMaxFps(engine_->GetMaxFps() / 2);
	}
	if (key == Urho3D::KEY_G) {
		Game::getEnvironment()->flipTerrainShaderParam("GridEnable");
	}
	if (key == Urho3D::KEY_V) {
		Game::getEnvironment()->nextVisibilityType();
	}
}

void Main::HandleMouseModeRequest(Urho3D::StringHash /*eventType*/, Urho3D::VariantMap& eventData) {
	const auto console = GetSubsystem<Urho3D::Console>();
	if (console && console->IsVisible()) { return; }
	auto input = GetSubsystem<Urho3D::Input>();
	if (useMouseMode_ == Urho3D::MM_ABSOLUTE) {
		input->SetMouseVisible(false);
	} else if (useMouseMode_ == Urho3D::MM_FREE) {
		input->SetMouseVisible(true);
	}
	input->SetMouseMode(useMouseMode_);
}

void Main::HandleMouseModeChange(Urho3D::StringHash /*eventType*/, Urho3D::VariantMap& eventData) {
	bool mouseLocked = eventData[Urho3D::MouseModeChanged::P_MOUSELOCKED].GetBool();
	GetSubsystem<Urho3D::Input>()->SetMouseVisible(!mouseLocked);
}

void Main::HandleSaveScene(Urho3D::StringHash /*eventType*/, Urho3D::VariantMap& eventData) {
	const auto element = static_cast<Urho3D::UIElement*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	const auto data = static_cast<FileFormData*>(element->GetVar("file_data").GetVoidPtr());
	save(data->fileName);
}

void Main::SetupViewport() {
	auto v = new Urho3D::Viewport(context_, Game::getScene(), Game::getCameraManager()->getComponent());
	Urho3D::SharedPtr<Urho3D::Viewport> viewport(v);

	//v->GetRenderPath()->Append(Game::getCache()->GetResource<XMLFile>("PostProcess/FXAA2.xml"));

	const auto renderer = GetSubsystem<Urho3D::Renderer>();
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

SelectedInfo* Main::control(const float timeStep, FrameInfo* frameInfo) {
	const Urho3D::IntVector2 cursorPos = Game::getUI()->GetCursorPosition();
	Urho3D::UIElement* element = Game::getUI()->GetElementAt(hud->getRoot(), cursorPos, true);

	if (element) {
		controls->deactivate();
	} else {
		controls->activate();
		controls->control();
	}

	const auto input = GetSubsystem<Urho3D::Input>();
	debugManager.change(input, simulation);
	if (input->GetKeyPress(Urho3D::KEY_P)) {
		Game::getEnvironment()->drawInfluence();
	}

	Game::getCameraManager()->translate(cursorPos, input, timeStep);

	controls->cleanAndUpdate(frameInfo);
	auto camera = Game::getCameraManager()->getComponent();
	healthBarProvider.reset(controls->getSelected().size(), controls->getInfo()->getSelectedType());
	for (auto selected : controls->getSelected()) {
		auto progressBar = healthBarProvider.getNext(selected->getType());

		if (progressBar) {
			auto pos = selected->getPosition();
			pos.y_ += selected->getModelHeight() * 1.3f;
			Urho3D::IntVector2 pixel{
				VectorRoundToInt(Urho3D::Vector2(Game::getGraphics()->GetSize()) * camera->WorldToScreenPoint(pos))
			};

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
	auto const& arguments = Urho3D::GetArguments();

	for (unsigned i = 0; i < arguments.Size(); ++i) {
		auto& current = arguments[i];
		if (current.Length() > 1 && current[0] == '-') {
			Urho3D::String argument = current.Substring(1).ToLower();
			const Urho3D::String value = i + 1 < arguments.Size() ? arguments[i + 1] : Urho3D::String::EMPTY;

			if (argument == "benchmark") {
				SimGlobals::BENCHMARK_MODE = true;
				engine_->SetMaxFps(0);
				engine_->SetMaxInactiveFps(0);
			} else if (argument == "headless") {
				SimGlobals::HEADLESS = true;
			} else if (argument == "framepack" && !value.Empty()) {
				SimGlobals::FRAME_PACK = ToInt(value);
				++i;
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
				engineParameters_[Urho3D::EP_WINDOW_WIDTH] = ToInt(value);
				++i;
			} else if (argument == "y" && !value.Empty()) {
				engineParameters_[Urho3D::EP_WINDOW_HEIGHT] = ToInt(value);
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
	auto const& arguments = Urho3D::GetArguments();

	for (unsigned i = 0; i < arguments.Size(); ++i) {
		if (arguments[i].Length() > 1 && arguments[i][0] == '-') {
			Urho3D::String argument = arguments[i].Substring(1).ToLower();
			const Urho3D::String value = i + 1 < arguments.Size() ? arguments[i + 1] : Urho3D::String::EMPTY;

			if (argument == "databasename" && !value.Empty()) {
				SimGlobals::DATABASE_NUMBER = value;
				++i;
			} else if (argument == "headless") {
				SimGlobals::HEADLESS = true;
			}
		}
	}
}
