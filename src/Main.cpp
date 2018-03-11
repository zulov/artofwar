#include "Urho3D/Resource/Image.h"
#include "camera/CameraEnums.h"
#include "commands/creation/CreationCommandList.h"
#include "database/DatabaseCache.h"
#include "hud/HudData.h"
#include "hud/window/in_game_menu/middle/FileFormData.h"
#include "hud/window/main_menu/new_game/NewGameForm.h"
#include "objects/LinkComponent.h"
#include "player/PlayersManager.h"
#include "simulation/SimulationInfo.h"
#include <Main.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Console.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Engine/EngineDefs.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Resource/Localization.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>


URHO3D_DEFINE_APPLICATION_MAIN(Main)

Main::Main(Context* context) : Application(context), useMouseMode_(MM_ABSOLUTE), saver(100) {
	gameState = GameState::STARTING;
	context->RegisterFactory<LinkComponent>();
	MySprite::RegisterObject(context);
	Game::init();
}

void Main::Setup() {
	Game* game = Game::get();

	game->setDatabaseCache(new DatabaseCache());
	db_settings* settings = game->getDatabaseCache()->getSettings();
	db_graph_settings* graphSettings = game->getDatabaseCache()->getGraphSettings(settings->graph);
	db_resolution* resolution = game->getDatabaseCache()->getResolution(settings->resolution);
	engineParameters_[EP_WINDOW_TITLE] = GetTypeName();
	engineParameters_[EP_LOG_NAME] = GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs") + GetTypeName() +
		".log";
	engineParameters_[EP_FULL_SCREEN] = graphSettings->fullscreen;
	engineParameters_[EP_HEADLESS] = false;
	engineParameters_[EP_SOUND] = false;
	engineParameters_[EP_WINDOW_WIDTH] = resolution->x;
	engineParameters_[EP_WINDOW_HEIGHT] = resolution->y;

	engine_->SetMaxFps(graphSettings->max_fps);
	engine_->SetMinFps(graphSettings->min_fps);

	game->setCache(GetSubsystem<ResourceCache>())->setUI(GetSubsystem<UI>())->
	      setConsole(GetSubsystem<Console>())->setContext(context_)->setEngine(engine_);
	loadingProgress.reset(loadStages);
	newGameProgress.reset(newGamesStages);
}

void Main::Start() {
	Game::get()->setGraphics(GetSubsystem<Graphics>());

	SetWindowTitleAndIcon();
	InitLocalizationSystem();

	subscribeToEvents();

	hud = new Hud();
	hud->preapreUrho();
	hud->createMyPanels();
	subscribeToUIEvents();
	InitMouseMode(MM_RELATIVE);
	changeState(GameState::LOADING);
}

void Main::Stop() {
	disposeScene();

	hud->clear();
	delete hud;
	Game::dispose();
	engine_->DumpResources(true);
}

void Main::subscribeToUIEvents() {
	for (auto hudElement : hud->getButtonsLeftMenuToSubscribe()) {
		UIElement* element = hudElement->getUIParent();
		SubscribeToEvent(element, E_CLICK, URHO3D_HANDLER(Main, HandleLeftMenuButton));
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
	benchmark.add(1.0 / timeStep);

	SimulationInfo* simulationInfo = simulation->update(timeStep);

	SelectedInfo* selectedInfo = control(timeStep, simulationInfo);

	hud->update(benchmark, Game::get()->getCameraManager(), selectedInfo, simulationInfo);

	simulation->dispose();
}

void Main::HandleUpdate(StringHash eventType, VariantMap& eventData) {
	switch (gameState) {
	case GameState::MENU_MAIN:
		//changeState(GameState::LOADING);
		break;
	case GameState::LOADING:
		load();
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
		newGame(newGameForm);
		break;
	case GameState::STARTING: break;
	default: ;
	}
}

void Main::InitMouseMode(MouseMode mode) {
	useMouseMode_ = mode;
	Input* input = GetSubsystem<Input>();

	Console* console = Game::get()->getConsole();
	if (useMouseMode_ != MM_ABSOLUTE) {
		input->SetMouseMode(useMouseMode_);
		if (console && console->IsVisible()) {
			input->SetMouseMode(MM_ABSOLUTE, true);
		}
	}
}

void Main::SetWindowTitleAndIcon() {
	Graphics* graphics = Game::get()->getGraphics();
	Image* icon = Game::get()->getCache()->GetResource<Image>("textures/UrhoIcon.png");
	graphics->SetWindowIcon(icon);
	graphics->SetWindowTitle("Art of War 2017");
}

void Main::changeCamera(int type) {
	Game::get()->getCameraManager()->setCameraBehave(type);
	SetupViewport();
	InitMouseMode(Game::get()->getCameraManager()->getMouseMode());
}

void Main::InitLocalizationSystem() {
	Localization* l10n = GetSubsystem<Localization>();

	l10n->LoadJSONFile("lang/language.json");
	Game::get()->setLocalization(l10n);
}

void Main::save(String name) {
	saver.createSave(name);
	saver.saveConfig();
	simulation->save(saver);
	Game::get()->getPlayersManager()->save(saver);
	saver.close();
}

void Main::createSimulation() {
	Game::get()->setCreationCommandList(new CreationCommandList());
	simulation = new Simulation(Game::get()->getEnviroment(), Game::get()->getCreationCommandList());
}

void Main::load() {
	switch (loadingProgress.currentStage) {
	case 0:
		{
		Game::get()->setCameraManager(new CameraManager());
		Game::get()->setQueueManager(new QueueManager(1));
		Game::get()->setFormationManager(new FormationManager());

		loader.createLoad(saveToLoad);
		levelBuilder = new LevelBuilder();
		controls = new Controls(GetSubsystem<Input>());
		SetupViewport();
		Game::get()->setPlayersManager(new PlayersManager());
		Game::get()->getPlayersManager()->load(loader.loadPlayers(), loader.loadResources());

		subscribeToUIEvents();
		hud->resetLoading();

		levelBuilder->createScene(loader);
		}
		break;
	case 1:
		{
		Game::get()->setEnviroment(new Enviroment(levelBuilder->getTerrian()));
		break;
		}
	case 2:
		{
		Game::get()->getEnviroment()->prepareGridToFind();
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
	loadingProgress.inc();
	hud->updateLoading(loadingProgress.getProgres());

}

void Main::newGame(NewGameForm* form) {
	switch (newGameProgress.currentStage) {
	case 0:
		{
		disposeScene();
		Game::get()->setCameraManager(new CameraManager());
		Game::get()->setQueueManager(new QueueManager(1));

		levelBuilder = new LevelBuilder();
		controls = new Controls(GetSubsystem<Input>());
		SetupViewport();
		Game::get()->setPlayersManager(new PlayersManager());
		Game::get()->getPlayersManager()->load(form);

		hud->resetLoading();

		levelBuilder->createScene(form);
		}
		break;
	case 1:
		{
		Game::get()->setEnviroment(new Enviroment(levelBuilder->getTerrian()));
		break;
		}
	case 2:
		{
		Game::get()->getEnviroment()->prepareGridToFind();
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
	newGameProgress.inc();
	hud->updateLoading(newGameProgress.getProgres());
}

void Main::changeState(GameState newState) {
	gameState = newState;
	hud->updateStateVisibilty(newState);
}

void Main::HandleKeyUp(StringHash /*eventType*/, VariantMap& eventData) {
	int key = eventData[KeyUp::P_KEY].GetInt();

	if (key == KEY_ESCAPE) {
		Console* console = GetSubsystem<Console>();
		if (console->IsVisible()) {
			console->SetVisible(false);
		} else {
			engine_->Exit();
		}
	}
	if (gameState == GameState::RUNNING || gameState == GameState::PAUSE) {
		if (key == KEY_1) {
			changeCamera(CameraBehaviorType::FREE);
		} else if (key == KEY_2) {
			changeCamera(CameraBehaviorType::RTS);
		} else if (key == KEY_3) {
			changeCamera(CameraBehaviorType::TOP);
		} else if (key == KEY_F5) {
			String name = "test" + String(rand());
			save(name);
		} else if (key == KEY_F9) {
			saveToLoad = "quicksave.db";

			changeState(GameState::CLOSING);
		}
	}
}

void Main::HandleNewGame(StringHash eventType, VariantMap& eventData) {
	UIElement* element = static_cast<UIElement*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	NewGameForm* form = static_cast<NewGameForm *>(element->GetVar("NewGameForm").GetVoidPtr());

	changeState(GameState::NEW_GAME);
	newGameForm = new NewGameForm(*form);
}

void Main::HandleLoadGame(StringHash eventType, VariantMap& eventData) {
	UIElement* element = static_cast<UIElement*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	String fileName = element->GetVar("LoadFileName").GetString();

	changeState(GameState::LOADING);

	saveToLoad = String(fileName);
}

void Main::HandleCloseGame(StringHash eventType, VariantMap& eventData) {
	engine_->Exit();
}

void Main::HandleLeftMenuButton(StringHash eventType, VariantMap& eventData) {
	//TODO need refactor
	UIElement* element = static_cast<UIElement*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	HudData* hudData = static_cast<HudData *>(element->GetVar("HudElement").GetVoidPtr());
	switch (hudData->getType()) {
	case LeftMenuAction::NONE: break;
	case LeftMenuAction::UNIT:
		controls->order(hudData->getId(), ActionParameter(ActionType::UNIT));
		break;
	case LeftMenuAction::UNIT_LEVEL:
		controls->order(hudData->getId(), ActionParameter(ActionType::UNIT_LEVEL));
		break;
	case LeftMenuAction::UNIT_UPGRADE:
		controls->order(hudData->getId(), ActionParameter(ActionType::UNIT_UPGRADE));
		break;
	case LeftMenuAction::BUILDING:
		controls->hudAction(hudData);
		break;
	case LeftMenuAction::BUILDING_LEVEL:
		controls->order(hudData->getId(), ActionParameter(ActionType::BUILDING_LEVEL));
		break;
	case LeftMenuAction::BUILDING_UPGRADE: break;
	case LeftMenuAction::ORDER:
		controls->order(hudData->getId(), ActionParameter(ActionType::ORDER));
		break;
	case LeftMenuAction::FORMATION:
		controls->order(hudData->getId(), ActionParameter(ActionType::FORMATION));
		break;
	default: ;
	}
}

void Main::HandleSelectedButton(StringHash eventType, VariantMap& eventData) {
	UIElement* element = static_cast<UIElement*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	SelectedHudElement* sHudElement = static_cast<SelectedHudElement *>(element->GetVar("SelectedHudElement").GetVoidPtr()
	);
	std::vector<Physical*>* selected = sHudElement->getSelected();
	controls->unSelectAll();
	for (auto physical : *selected) {
		controls->select(physical);
	}
}

void Main::HandleKeyDown(StringHash /*eventType*/, VariantMap& eventData) {
	const int key = eventData[KeyDown::P_KEY].GetInt();

	if (key == KEY_F1) {
		GetSubsystem<Console>()->Toggle();
	} else if (key == KEY_F2) {
		GetSubsystem<DebugHud>()->ToggleAll();
	}

}

void Main::HandleMouseModeRequest(StringHash /*eventType*/, VariantMap& eventData) {
	Console* console = GetSubsystem<Console>();
	if (console && console->IsVisible()) { return; }
	Input* input = GetSubsystem<Input>();
	if (useMouseMode_ == MM_ABSOLUTE) {
		input->SetMouseVisible(false);
	} else if (useMouseMode_ == MM_FREE) {
		input->SetMouseVisible(true);
	}
	input->SetMouseMode(useMouseMode_);
}

void Main::HandleMouseModeChange(StringHash /*eventType*/, VariantMap& eventData) {
	Input* input = GetSubsystem<Input>();
	bool mouseLocked = eventData[MouseModeChanged::P_MOUSELOCKED].GetBool();
	input->SetMouseVisible(!mouseLocked);
}

void Main::HandleSaveScene(StringHash /*eventType*/, VariantMap& eventData) {
	UIElement* element = static_cast<UIElement*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	FileFormData* data = static_cast<FileFormData *>(element->GetVar("file_data").GetVoidPtr());
	save(data->fileName);
}

void Main::SetupViewport() {
	SharedPtr<Viewport> viewport(new Viewport(context_, Game::get()->getScene(),
	                                          Game::get()->getCameraManager()->getComponent()));
	GetSubsystem<Renderer>()->SetViewport(0, viewport);
}

void Main::disposeScene() {
	if (inited) {
		loading loading2;
		Game::get()->getScene()->SetUpdateEnabled(false);

		loading2.reset(4, "dispose simulation");
		delete simulation;
		simulation = nullptr;

		loading2.inc("dispose managers");
		delete Game::get()->getCameraManager();
		Game::get()->setCameraManager(nullptr);

		delete Game::get()->getQueueManager();
		Game::get()->setQueueManager(nullptr);

		delete Game::get()->getFormationManager();
		Game::get()->setFormationManager(nullptr);

		loading2.inc("dispose creationList");
		delete Game::get()->getCreationCommandList();
		Game::get()->setCreationCommandList(nullptr);

		loading2.inc("dispose enviroment");
		delete Game::get()->getEnviroment();
		Game::get()->setEnviroment(nullptr);

		loading2.inc("dispose playerManager");
		delete Game::get()->getPlayersManager();
		Game::get()->setPlayersManager(nullptr);

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

SelectedInfo* Main::control(const float timeStep, SimulationInfo* simulationInfo) const {
	const IntVector2 cursorPos = Game::get()->getUI()->GetCursorPosition();
	UIElement* element = Game::get()->getUI()->GetElementAt(cursorPos, false);

	if (element) {
		controls->deactivate();
	} else {
		controls->activate();
		controls->control();
	}

	Input* input = GetSubsystem<Input>();

	Game::get()->getCameraManager()->translate(cursorPos, input, timeStep);
	Game::get()->getCameraManager()->rotate(input->GetMouseMove());

	controls->clean(simulationInfo);
	return controls->getInfo();
}
