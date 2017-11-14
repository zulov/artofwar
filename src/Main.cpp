#include <Main.h>
#include "simulation/SimulationInfo.h"
#include <iostream>
#include "database/DatabaseCache.h"
#include "player/PlayersManager.h"
#include "Urho3D/Resource/Image.h"

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

	game->setCache(GetSubsystem<ResourceCache>())->setUI(GetSubsystem<UI>())->
	      setConsole(GetSubsystem<Console>())->setContext(context_)->setEngine(engine_);
	loadingState->reset(4);
}

void Main::load() {
	switch (loadingState->currentStage) {
	case 0:
		{
		hud->resetLoading();
		levelBuilder->createScene(1);
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
		enviroment->prepareGridToFind();

		break;
		}
	case 3:
		Game::get()->setCreationCommandList(new CreationCommandList());
		simulation = new Simulation(static_cast<Enviroment*>(loadingState->sth), Game::get()->getCreationCommandList());
		break;
	case 4:
		simulation->initScene();
		break;
	case 5:
		gameState = GameState::RUNNING;
		hud->endLoading();
		break;
	}
	loadingState->inc();
	hud->updateLoading(loadingState->getProgres());

}

void Main::Start() {
	Game* game = Game::get();
	game->setGraphics(GetSubsystem<Graphics>());
	game->setPlayersManager(new PlayersManager());

	SetWindowTitleAndIcon();

	benchmark = new Benchmark();
	hud = new Hud();
	subscribeToEvents();

	game->setCameraManager(new CameraManager());
	levelBuilder = new LevelBuilder(new SceneObjectManager());
	SetupViewport();
	InitMouseMode(MM_RELATIVE);
	controls = new Controls(GetSubsystem<Input>());
	cameraManager = game->getCameraManager();

	gameState = GameState::MENU;

}

void Main::Stop() {
	engine_->DumpResources(true);
	delete loadingState;
}

void Main::subscribeToEvents() {
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

	for (auto buttton : *hud->getButtonsQueueToSubscribe()) {
		SubscribeToEvent(buttton, E_CLICK, URHO3D_HANDLER(Main, HandleQueueButton));
		//SubscribeToEvent(buttton, E_CLICK, URHO3D_HANDLER(Main, HandleQueueButton));//TODO zrobic z prawym przyciskiem tez
	}

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
	case GameState::ENDING: break;

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
	}
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

void Main::HandleQueueButton(StringHash eventType, VariantMap& eventData) {
	UIElement* element = (UIElement*)eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr();
	QueueHudElement* qHudElement = (QueueHudElement *)element->GetVar("QueueHudElement").GetVoidPtr();
	qHudElement->reduce(1);
}

void Main::HandleKeyDown(StringHash /*eventType*/, VariantMap& eventData) {
	using namespace KeyDown;

	int key = eventData[P_KEY].GetInt();

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

void Main::SetupViewport() {
	SharedPtr<Viewport> viewport(new Viewport(context_, Game::get()->getScene(),
	                                          Game::get()->getCameraManager()->getComponent()));
	GetSubsystem<Renderer>()->SetViewport(0, viewport);
}

void Main::control(float timeStep) {
	IntVector2 cursorPos = Game::get()->getUI()->GetCursorPosition();
	UIElement* element = GetSubsystem<UI>()->GetElementAt(cursorPos, false);

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
