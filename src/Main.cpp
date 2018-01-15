#include <Main.h>
#include "simulation/SimulationInfo.h"
#include "database/DatabaseCache.h"
#include "player/PlayersManager.h"
#include "Urho3D/Resource/Image.h"
#include "camera/CameraEnums.h"
#include "objects/LinkComponent.h"
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/EngineDefs.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Engine/Console.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Resource/Localization.h>
#include "commands/creation/CreationCommandList.h"
#include "hud/window/in_game_menu/middle/FileFormData.h"
#include "hud/window/main_menu/new_game/NewGameForm.h"

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

	db_graph_settings* graphSettings = game->getDatabaseCache()->getGraphSettings(0);

	engineParameters_[EP_WINDOW_TITLE] = GetTypeName();
	engineParameters_[EP_LOG_NAME] = GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs") + GetTypeName() +
		".log";
	engineParameters_[EP_FULL_SCREEN] = graphSettings->fullscreen;
	engineParameters_[EP_HEADLESS] = false;
	engineParameters_[EP_SOUND] = false;
	engineParameters_[EP_WINDOW_HEIGHT] = graphSettings->res_y;
	engineParameters_[EP_WINDOW_WIDTH] = graphSettings->res_x;

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
	benchmark = new Benchmark();

	subscribeToEvents();

	hud = new Hud();
	hud->preapreUrho();
	InitMouseMode(MM_RELATIVE);
	gameState = GameState::MENU;
}

void Main::Stop() {
	disposeScene();

	delete benchmark;
	delete hud;
	Game::dispose();
	engine_->DumpResources(true);
}

void Main::subscribeToUIEvents() {
	for (auto hudElement : hud->getButtonsBuildToSubscribe()) {
		UIElement* element = hudElement->getUIElement();
		SubscribeToEvent(element, E_CLICK, URHO3D_HANDLER(Main, HandleBuildButton));
	}

	for (auto hudElement : hud->getButtonsUnitsToSubscribe()) {
		UIElement* element = hudElement->getUIElement();
		SubscribeToEvent(element, E_CLICK, URHO3D_HANDLER(Main, HandleUnitButton));
	}

	for (auto hudElement : hud->getButtonsOrdersToSubscribe()) {
		UIElement* element = hudElement->getUIElement();
		SubscribeToEvent(element, E_CLICK, URHO3D_HANDLER(Main, HandleOrdersButton));
	}

	for (auto buttton : hud->getButtonsSelectedToSubscribe()) {
		SubscribeToEvent(buttton, E_CLICK, URHO3D_HANDLER(Main, HandleSelectedButton));
	}

	SubscribeToEvent(hud->getSaveButton(), E_CLICK, URHO3D_HANDLER(Main, HandleSaveScene));
	SubscribeToEvent(hud->getNewGameProceed(), E_CLICK, URHO3D_HANDLER(Main, HandleNewGame));

	Sprite* minimap = hud->getSpriteMiniMapToSubscribe();
	SubscribeToEvent(minimap, E_CLICK, URHO3D_HANDLER(Main, HandleMiniMapClick));

	hud->subscribeToUIEvents();
}

void Main::subscribeToEvents() {
	SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(Main, HandleKeyDown));
	SubscribeToEvent(E_KEYUP, URHO3D_HANDLER(Main, HandleKeyUp));
	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Main, HandleUpdate));
}

void Main::running(VariantMap& eventData) {
	const double timeStep = eventData[SceneUpdate::P_TIMESTEP].GetDouble();
	benchmark->add(1.0 / timeStep);

	simulation->update(GetSubsystem<Input>(), timeStep);
	SimulationInfo* simulationInfo = simulation->getInfo();
	control(timeStep);
	controls->clean(simulationInfo);

	SelectedInfo* selectedInfo = controls->getInfo();

	hud->updateSelected(selectedInfo);
	if (simulationInfo->ifAmountUnitChanged()) {
		hud->update(simulation->getUnitsNumber());
	}

	hud->update(benchmark, Game::get()->getCameraManager());

	selectedInfo->hasBeedUpdatedDrawn();

	simulation->dispose();
}

void Main::HandleUpdate(StringHash eventType, VariantMap& eventData) {
	switch (gameState) {
	case GameState::MENU:
		gameState = GameState::LOADING;
		break;
	case GameState::LOADING:
		load();
		break;
	case GameState::RUNNING:
		running(eventData);
		break;
	case GameState::PAUSE: break;
	case GameState::CLOSING:
		disposeScene();
		gameState = GameState::MENU;
		break;
	case GameState::NEW_GAME:
		newGame(newGameForm);
		break;
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

void Main::load() {
	switch (loadingProgress.currentStage) {
	case 0:
		{
		Game::get()->setCameraManager(new CameraManager());

		controls = new Controls(GetSubsystem<Input>());
		loader.createLoad(saveToLoad);
		levelBuilder = new LevelBuilder();
		SetupViewport();
		Game::get()->setPlayersManager(new PlayersManager());
		Game::get()->getPlayersManager()->load(loader.loadPlayers(), loader.loadResources());

		hud->createMyPanels();
		subscribeToUIEvents();
		hud->resetLoading();

		levelBuilder->createScene(loader);
		}
		break;
	case 1:
		{
		Enviroment* enviroment = new Enviroment(levelBuilder->getTerrian());
		Game::get()->setEnviroment(enviroment);
		break;
		}
	case 2:
		{
		//Game::get()->getEnviroment()->prepareGridToFind();
		hud->createMiniMap();
		break;
		}
	case 3:
		Game::get()->setCreationCommandList(new CreationCommandList());
		simulation = new Simulation(Game::get()->getEnviroment(), Game::get()->getCreationCommandList());
		break;
	case 4:
		simulation->initScene(loader);
		break;
	case 5:
		gameState = GameState::RUNNING;
		loader.end();
		hud->endLoading();
		break;
	}
	loadingProgress.inc();
	hud->updateLoading(loadingProgress.getProgres());

}

void Main::newGame(NewGameForm * form) {
	switch (newGameProgress.currentStage) {
	case 0:
		{
		Game::get()->setCameraManager(new CameraManager());

		controls = new Controls(GetSubsystem<Input>());
		
		levelBuilder = new LevelBuilder();
		SetupViewport();
		Game::get()->setPlayersManager(new PlayersManager());
		Game::get()->getPlayersManager()->load(form);

		hud->createMyPanels();
		subscribeToUIEvents();
		hud->resetLoading();

		levelBuilder->createScene(form);
		}
		break;
	case 1:
		{
		Enviroment* enviroment = new Enviroment(levelBuilder->getTerrian());
		Game::get()->setEnviroment(enviroment);
		break;
		}
	case 2:
		{
		//Game::get()->getEnviroment()->prepareGridToFind();
		hud->createMiniMap();
		break;
		}
	case 3:
		Game::get()->setCreationCommandList(new CreationCommandList());
		simulation = new Simulation(Game::get()->getEnviroment(), Game::get()->getCreationCommandList());
		break;
	case 4:
		simulation->initScene(form);
		break;
	case 5:
		gameState = GameState::RUNNING;
		
		hud->endLoading();
		break;
	}
	newGameProgress.inc();
	hud->updateLoading(newGameProgress.getProgres());
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
	} else if (key == KEY_1) {
		changeCamera(CameraBehaviorType::FREE);
	} else if (key == KEY_2) {
		changeCamera(CameraBehaviorType::RTS);
	} else if (key == KEY_3) {
		changeCamera(CameraBehaviorType::TOP);
	} else if (key == KEY_F5) {
		String name = "test" + String(rand());
		save(name);
	} else if (key == KEY_F9) {
		saveToLoad = "quicksave";
		gameState = GameState::CLOSING;
	}
}

void Main::HandleNewGame(StringHash eventType, VariantMap& eventData) {
	UIElement* element = static_cast<UIElement*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	NewGameForm* form = static_cast<NewGameForm *>(element->GetVar("NewGameForm").GetVoidPtr());
	gameState = GameState::NEW_GAME;
	newGameForm = form;

}

void Main::HandleMiniMapClick(StringHash eventType, VariantMap& eventData) {
	Sprite* element = static_cast<Sprite*>(eventData[Urho3D::Click::P_ELEMENT].GetVoidPtr());
	IntVector2 begin = element->GetScreenPosition();
	IntVector2 size = element->GetSize();
	float x = eventData[Urho3D::Click::P_X].GetInt() - begin.x_;
	float y = size.y_ - (eventData[Urho3D::Click::P_Y].GetInt() - begin.y_);

	Game::get()->getCameraManager()->changePosition(x / size.x_, y / size.y_);
}

void Main::HandleBuildButton(StringHash eventType, VariantMap& eventData) {
	UIElement* element = static_cast<UIElement*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	HudElement* hudElement = static_cast<HudElement *>(element->GetVar("HudElement").GetVoidPtr());
	controls->hudAction(hudElement);
}

void Main::HandleUnitButton(StringHash eventType, VariantMap& eventData) {
	UIElement* element = static_cast<UIElement*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	HudElement* hudElement = static_cast<HudElement *>(element->GetVar("HudElement").GetVoidPtr());

	controls->order(hudElement->getId());
}

void Main::HandleOrdersButton(StringHash eventType, VariantMap& eventData) {
	UIElement* element = static_cast<UIElement*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	HudElement* hudElement = static_cast<HudElement *>(element->GetVar("HudElement").GetVoidPtr());

	controls->order(hudElement->getId());
}

void Main::HandleSelectedButton(StringHash eventType, VariantMap& eventData) {
	UIElement* element = static_cast<UIElement*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	SelectedHudElement* sHudElement = static_cast<SelectedHudElement *>(element->GetVar("SelectedHudElement").GetVoidPtr()
	);
	std::vector<Physical*>* selected = sHudElement->getSelected();
	controls->unSelectAll();
	for (auto physical : (*selected)) {
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
	loading loading2;
	Game::get()->getScene()->SetUpdateEnabled(false);

	loading2.reset(4, "dispose simulation");
	delete simulation;
	simulation = nullptr;

	loading2.inc("dispose cameras");
	delete Game::get()->getCameraManager();
	Game::get()->setCameraManager(nullptr);

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

	loading2.inc("dispose hud");
	hud->clear();

	loading2.inc("dispose levelBuilder");
	delete levelBuilder;
	levelBuilder = nullptr;

	loadingProgress.reset(loadStages);
	newGameProgress.reset(newGamesStages);
}

void Main::control(const float timeStep) const {
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
}
