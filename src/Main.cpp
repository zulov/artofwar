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
#include "hud/window/middle/FileFormData.h"
#include "commands/creation/CreationCommandList.h"

URHO3D_DEFINE_APPLICATION_MAIN(Main)

Main::Main(Context* context) : Application(context), useMouseMode_(MM_ABSOLUTE) {
	gameState = GameState::STARTING;
	context->RegisterFactory<LinkComponent>();
	MySprite::RegisterObject(context);
	Game::init();
	loadingState = new loading();
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

	saver = new SceneSaver(100);
	loader = new SceneLoader();

	game->setCache(GetSubsystem<ResourceCache>())->setUI(GetSubsystem<UI>())->
	      setConsole(GetSubsystem<Console>())->setContext(context_)->setEngine(engine_);
	loadingState->reset(4);
}

void Main::load() {
	switch (loadingState->currentStage) {
	case 0:
		{
		Game::get()->setCameraManager(new CameraManager());
		cameraManager = Game::get()->getCameraManager();
		controls = new Controls(GetSubsystem<Input>());
		loader->createLoad("quicksave");
		levelBuilder = new LevelBuilder(new SceneObjectManager());
		SetupViewport();
		Game::get()->setPlayersManager(new PlayersManager());
		Game::get()->getPlayersManager()->load(loader->loadPlayers(), loader->loadResources());
		hud = new Hud();
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
		loadingState->sth = enviroment;
		break;
		}
	case 2:
		{
		Enviroment* enviroment = static_cast<Enviroment*>(loadingState->sth);
		//enviroment->prepareGridToFind();
		hud->createMiniMap();
		break;
		}
	case 3:
		Game::get()->setCreationCommandList(new CreationCommandList());
		simulation = new Simulation(static_cast<Enviroment*>(loadingState->sth), Game::get()->getCreationCommandList());
		break;
	case 4:
		simulation->initScene(loader);
		break;
	case 5:
		gameState = GameState::RUNNING;
		loader->end();
		hud->endLoading();
		break;
	}
	loadingState->inc();
	hud->updateLoading(loadingState->getProgres());

}

void Main::Start() {
	Game* game = Game::get();
	game->setGraphics(GetSubsystem<Graphics>());

	SetWindowTitleAndIcon();
	InitLocalizationSystem();
	benchmark = new Benchmark();

	subscribeToEvents();

	InitMouseMode(MM_RELATIVE);

	gameState = GameState::MENU;
}

void Main::Stop() {
	engine_->DumpResources(true);
	delete loadingState;
	delete loader;
	delete saver;
	delete benchmark;
	Game::dispose();
}

void Main::subscribeToUIEvents() {
	for (auto hudElement : *hud->getButtonsBuildToSubscribe()) {
		UIElement* element = hudElement->getUIElement();
		SubscribeToEvent(element, E_CLICK, URHO3D_HANDLER(Main, HandleBuildButton));
		SubscribeToEvent(element, E_HOVERBEGIN, URHO3D_HANDLER(Main, HandleUIButtonHoverOn));
		SubscribeToEvent(element, E_HOVEREND, URHO3D_HANDLER(Main, HandleUIButtonHoverOff));
	}

	for (auto hudElement : *hud->getButtonsUnitsToSubscribe()) {
		UIElement* element = hudElement->getUIElement();
		SubscribeToEvent(element, E_CLICK, URHO3D_HANDLER(Main, HandleUnitButton));
		SubscribeToEvent(element, E_HOVERBEGIN, URHO3D_HANDLER(Main, HandleUIButtonHoverOn));
		SubscribeToEvent(element, E_HOVEREND, URHO3D_HANDLER(Main, HandleUIButtonHoverOff));
	}

	for (auto hudElement : *hud->getButtonsOrdersToSubscribe()) {
		UIElement* element = hudElement->getUIElement();
		SubscribeToEvent(element, E_CLICK, URHO3D_HANDLER(Main, HandleOrdersButton));
		SubscribeToEvent(element, E_HOVERBEGIN, URHO3D_HANDLER(Main, HandleUIButtonHoverOn));
		SubscribeToEvent(element, E_HOVEREND, URHO3D_HANDLER(Main, HandleUIButtonHoverOff));
	}

	for (auto buttton : *hud->getButtonsSelectedToSubscribe()) {
		SubscribeToEvent(buttton, E_CLICK, URHO3D_HANDLER(Main, HandleSelectedButton));
	}

	SubscribeToEvent(hud->getSaveButton(), E_CLICK, URHO3D_HANDLER(Main, HandleSaveScene));

	Sprite* minimap = hud->getSpriteMiniMapToSubscribe();
	SubscribeToEvent(minimap, E_CLICK, URHO3D_HANDLER(Main, HandleMiniMapClick));
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

	hud->update(benchmark, cameraManager);

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
		diposeScene();
		gameState = GameState::MENU;
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
	cameraManager->setCameraBehave(type);
	SetupViewport();
	InitMouseMode(cameraManager->getMouseMode());
}

void Main::InitLocalizationSystem() {
	Localization* l10n = GetSubsystem<Localization>();

	l10n->LoadJSONFile("lang/language.json");
	Game::get()->setLocalization(l10n);
}

void Main::save(String name) {
	saver->createSave(name);
	saver->saveConfig();
	simulation->save(saver);
	Game::get()->getPlayersManager()->save(saver);
	saver->close();
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
	} else if (key == KEY_F6) {
		String name = "quicksave";
		loadSave(name);
	} else if (key == KEY_F9) {
		gameState = GameState::CLOSING;
	}
}

void Main::HandleMiniMapClick(StringHash eventType, VariantMap& eventData) {
	Sprite* element = (Sprite*)eventData[Urho3D::Click::P_ELEMENT].GetVoidPtr();
	IntVector2 begin = element->GetScreenPosition();
	IntVector2 size = element->GetSize();
	float x = eventData[Urho3D::Click::P_X].GetInt() - begin.x_;
	float y = size.y_ - (eventData[Urho3D::Click::P_Y].GetInt() - begin.y_);

	cameraManager->changePosition(x / size.x_, y / size.y_);
}

void Main::HandleBuildButton(StringHash eventType, VariantMap& eventData) {
	UIElement* element = (UIElement*)eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr();
	HudElement* hudElement = (HudElement *)element->GetVar("HudElement").GetVoidPtr();
	controls->hudAction(hudElement);
}

void Main::HandleUnitButton(StringHash eventType, VariantMap& eventData) {
	UIElement* element = (UIElement*)eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr();
	HudElement* hudElement = (HudElement *)element->GetVar("HudElement").GetVoidPtr();

	controls->order(hudElement->getId());
}

void Main::HandleOrdersButton(StringHash eventType, VariantMap& eventData) {
	UIElement* element = (UIElement*)eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr();
	HudElement* hudElement = (HudElement *)element->GetVar("HudElement").GetVoidPtr();

	controls->order(hudElement->getId());
}

void Main::HandleSelectedButton(StringHash eventType, VariantMap& eventData) {
	UIElement* element = (UIElement*)eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr();
	SelectedHudElement* sHudElement = (SelectedHudElement *)element->GetVar("SelectedHudElement").GetVoidPtr();
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

void Main::loadSave(const String& name) {
	loader->createLoad(name);
	//loader->load();
	std::vector<dbload_player*>* players = loader->loadPlayers();
	std::vector<dbload_resource*>* resources = loader->loadResources();

	std::vector<dbload_unit*>* units = loader->loadUnits();
	std::vector<dbload_building*>* buildings = loader->loadBuildings();
	std::vector<dbload_resource_entities*>* resources_entities = loader->loadResourcesEntities();

	loader->end();
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

void Main::HandleUIButtonHoverOn(StringHash /*eventType*/, VariantMap& eventData) {
	UIElement* element = (UIElement*)eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr();
	HudElement* hudElement = (HudElement *)element->GetVar("HudElement").GetVoidPtr();
	hud->hoverOnIcon(hudElement);
}

void Main::HandleUIButtonHoverOff(StringHash /*eventType*/, VariantMap& eventData) {
	UIElement* element = (UIElement*)eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr();
	HudElement* hudElement = (HudElement *)element->GetVar("HudElement").GetVoidPtr();
	hud->hoverOffIcon(hudElement);
}

void Main::HandleSaveScene(StringHash /*eventType*/, VariantMap& eventData) {
	UIElement* element = (UIElement*)eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr();
	FileFormData* data = (FileFormData *)element->GetVar("file_data").GetVoidPtr();
	save(data->fileName);
}

void Main::SetupViewport() {
	//	if (!GetSubsystem<Renderer>()->GetViewport(0)) {
	//		delete GetSubsystem<Renderer>()->GetViewport(0);
	//	}
	SharedPtr<Viewport> viewport(new Viewport(context_, Game::get()->getScene(),
	                                          Game::get()->getCameraManager()->getComponent()));
	GetSubsystem<Renderer>()->SetViewport(0, viewport);
}

void Main::diposeScene() {
	loading* loading2 = new loading();

	loading2->reset(4, "dispose simulation");
	delete simulation;
	simulation = nullptr;

	loading2->inc("dispose cameras");
	delete cameraManager;
	cameraManager = nullptr;
	Game::get()->setCameraManager(nullptr);

	loading2->inc("dispose creationList");
	delete Game::get()->getCreationCommandList();
	Game::get()->setCreationCommandList(nullptr);

	loading2->inc("dispose enviroment");
	delete Game::get()->getEnviroment();
	Game::get()->setEnviroment(nullptr);

	loading2->inc("dispose playerManager");
	delete Game::get()->getPlayersManager();
	Game::get()->setPlayersManager(nullptr);

	loading2->inc("dispose controls");
	delete controls;
	controls = nullptr;

	loading2->inc("dispose hud");
	delete hud;
	hud = nullptr;

	loading2->inc("dispose levelBuilder");
	delete levelBuilder;
	levelBuilder = nullptr;

	loadingState->reset(4);
	delete loading2;
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

	cameraManager->translate(cursorPos, input, timeStep);
	cameraManager->rotate(input->GetMouseMove());
}
