#include <Main.h>

URHO3D_DEFINE_APPLICATION_MAIN(Main)

Main::Main(Context* context) : Application(context), useMouseMode_(MM_ABSOLUTE) {
	benchmark = new Benchmark();
	context->RegisterFactory<LinkComponent>();
	MySprite::RegisterObject(context);
}

void Main::Setup() {
	Game* game = Game::get();
	game->setDatabaseCache(new DatabaseCache());
	db_graph_settings* graphSettings = game->getDatabaseCache()->getGraphSettings(0);

	engineParameters_[EP_WINDOW_TITLE] = GetTypeName();
	engineParameters_[EP_LOG_NAME] = GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs") + GetTypeName() + ".log";
	engineParameters_[EP_FULL_SCREEN] = graphSettings->fullscreen;
	engineParameters_[EP_HEADLESS] = false;
	engineParameters_[EP_SOUND] = false;
	engineParameters_[EP_WINDOW_HEIGHT] = graphSettings->res_y;
	engineParameters_[EP_WINDOW_WIDTH] = graphSettings->res_x;

	engine_->SetMaxFps(graphSettings->max_fps);
	engine_->SetMinFps(graphSettings->min_fps);
}

void Main::Start() {
	Game* game = Game::get();
	game->setCache(GetSubsystem<ResourceCache>())->setUI(GetSubsystem<UI>())->setGraphics(GetSubsystem<Graphics>())->setConsole(GetSubsystem<Console>())->setContext(context_)->setEngine(engine_);
	SetWindowTitleAndIcon();
	SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(Main, HandleKeyDown));
	SubscribeToEvent(E_KEYUP, URHO3D_HANDLER(Main, HandleKeyUp));
	game->setPlayersManager(new PlayersManager());
	hud = new Hud();

	for (HudElement* hudElement : *(hud->getButtonsBuildToSubscribe())) {
		UIElement * element = hudElement->getUIElement();
		SubscribeToEvent(element, E_CLICK, URHO3D_HANDLER(Main, HandleBuildButton));
		SubscribeToEvent(element, E_HOVERBEGIN, URHO3D_HANDLER(Main, HandleUIButtonHoverOn));
		SubscribeToEvent(element, E_HOVEREND, URHO3D_HANDLER(Main, HandleUIButtonHoverOff));
	}

	for (HudElement* hudElement : *(hud->getButtonsUnitsToSubscribe())) {
		UIElement * element = hudElement->getUIElement();
		SubscribeToEvent(element, E_CLICK, URHO3D_HANDLER(Main, HandleUnitButton));
		SubscribeToEvent(element, E_HOVERBEGIN, URHO3D_HANDLER(Main, HandleUIButtonHoverOn));
		SubscribeToEvent(element, E_HOVEREND, URHO3D_HANDLER(Main, HandleUIButtonHoverOff));
	}

	for (HudElement* hudElement : *(hud->getButtonsOrdersToSubscribe())) {
		UIElement * element = hudElement->getUIElement();
		SubscribeToEvent(element, E_CLICK, URHO3D_HANDLER(Main, HandleOrdersButton));
		SubscribeToEvent(element, E_HOVERBEGIN, URHO3D_HANDLER(Main, HandleUIButtonHoverOn));
		SubscribeToEvent(element, E_HOVEREND, URHO3D_HANDLER(Main, HandleUIButtonHoverOff));
	}

	for (Button* buttton : *(hud->getButtonsSelectedToSubscribe())) {
		SubscribeToEvent(buttton, E_CLICK, URHO3D_HANDLER(Main, HandleSelectedButton));
	}

	for (Window* window : *(hud->getWindows())) {
		SubscribeToEvent(window, E_CLICKEND, URHO3D_HANDLER(Main, HandleWindowClick));
		SubscribeToEvent(window, E_HOVERBEGIN, URHO3D_HANDLER(Main, HandleWindowClick));
		SubscribeToEvent(window, E_HOVEREND, URHO3D_HANDLER(Main, HandleEndWindowClick));
	}
	CreateConsoleAndDebugHud();

	sceneObjectManager = new SceneObjectManager();

	levelBuilder = new LevelBuilder(sceneObjectManager);

	BuildList* buildList = new BuildList();
	buildList->setSceneObjectManager(sceneObjectManager);

	cameraManager = new CameraManager();
	SimulationObjectManager* simulationObjectManager = new SimulationObjectManager();
	SimulationCommandList* simulationCommandList = new SimulationCommandList(simulationObjectManager);
	EnviromentStrategy* enviromentStrategy = new EnviromentStrategy();
	mediator = new Mediator(enviromentStrategy, controls);
	game->setScene(levelBuilder->createScene())->setCameraManager(cameraManager)->setBuildList(buildList)->setSimCommandList(simulationCommandList)->setMediator(mediator);

	simulation = new Simulation(enviromentStrategy, simulationCommandList, simulationObjectManager);

	SetupViewport();
	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Main, HandleUpdate));

	InitMouseMode(MM_RELATIVE);
	controls = new Controls(GetSubsystem<Input>());
}

void Main::Stop() {
	engine_->DumpResources(true);
}

void Main::HandleUpdate(StringHash eventType, VariantMap& eventData) {
	double timeStep = eventData[SceneUpdate::P_TIMESTEP].GetDouble();
	simulation->update(GetSubsystem<Input>(), timeStep);
	benchmark->add(1.0 / timeStep);
	hud->updateHud(benchmark, cameraManager);
	control(timeStep);
	SelectedInfo* selectedInfo = controls->getSelectedInfo();
	hud->updateSelected(selectedInfo);
	if (selectedInfo->hasChanged()) {
		controls->updateState(selectedInfo);
	}
	
	selectedInfo->hasBeedUpdatedDrawn();
	levelBuilder->execute();
}

void Main::HandleWindowClick(StringHash eventType, VariantMap& eventData) {
	controls->deactivate();
}

void Main::HandleEndWindowClick(StringHash eventType, VariantMap& eventData) {
	controls->activate();
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

void Main::CreateConsoleAndDebugHud() {
	ResourceCache* cache = Game::get()->getCache();
	XMLFile* xmlFile = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");

	hud->createConsole();
	hud->createDebugHud();

	GetSubsystem<Input>()->SetMouseVisible(true);
	GetSubsystem<UI>()->GetRoot()->SetDefaultStyle(xmlFile);
}

void Main::HandleKeyUp(StringHash /*eventType*/, VariantMap& eventData) {
	using namespace KeyUp;

	int key = eventData[P_KEY].GetInt();

	if (key == KEY_ESCAPE) {
		Console* console = GetSubsystem<Console>();
		if (console->IsVisible()) {
			console->SetVisible(false);
		} else {
			engine_->Exit();
		}
	} else if (key == KEY_1) {
		cameraManager->setCameraBehave(CameraBehaviorType::FREE);
		SetupViewport();
		InitMouseMode(cameraManager->getMouseMode());
	} else if (key == KEY_2) {
		cameraManager->setCameraBehave(CameraBehaviorType::RTS);
		SetupViewport();
		InitMouseMode(cameraManager->getMouseMode());
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
	controls->action(hudElement);
}

void Main::HandleOrdersButton(StringHash eventType, VariantMap& eventData) {
	UIElement* element = (UIElement*)eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr();
	HudElement* hudElement = (HudElement *)element->GetVar("HudElement").GetVoidPtr();
	controls->action(hudElement);
}

void Main::HandleSelectedButton(StringHash eventType, VariantMap& eventData) {
	UIElement* element = (UIElement*)eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr();
	SelectedHudElement* sHudElement = (SelectedHudElement *)element->GetVar("SelectedHudElement").GetVoidPtr();
	std::vector<Physical*>* selected = sHudElement->getSelected();
	controls->unSelect();
	for (auto physical :(*selected)) {
		controls->select(physical);
	}
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
	Renderer* renderer = GetSubsystem<Renderer>();

	SharedPtr<Viewport> viewport(new Viewport(context_, Game::get()->getScene(), cameraManager->getComponent()));
	renderer->SetViewport(0, viewport);
}

void Main::control(float timeStep) {
	//if (GetSubsystem<UI>()->GetFocusElement()) { return; }
	Input* input = GetSubsystem<Input>();

	bool cameraKeys[4] = {input->GetKeyDown(KEY_W), input->GetKeyDown(KEY_S), input->GetKeyDown(KEY_A), input->GetKeyDown(KEY_D)};
	int width = Game::get()->getGraphics()->GetWidth();
	int height = Game::get()->getGraphics()->GetHeight();
	IntVector2 cursorPos = Game::get()->getUI()->GetCursorPosition();
	float border = 256.f;
	if (cursorPos.x_ < width / border) {
		cameraKeys[2] = true;
	} else if (cursorPos.x_ > width - (width / border)) {
		cameraKeys[3] = true;
	}
	if (cursorPos.y_ < height / border) {
		cameraKeys[0] = true;
	} else if (cursorPos.y_ > height - (height / border)) {
		cameraKeys[1] = true;
	}
	int wheel = input->GetMouseMoveWheel();
	cameraManager->translate(cameraKeys, wheel, timeStep);
	cameraManager->rotate(input->GetMouseMove());

	if (controls->isActive()) {
		if (input->GetMouseButtonDown(MOUSEB_LEFT)) {
			controls->clickDown(MOUSEB_LEFT);
		} else {
			controls->release(MOUSEB_LEFT);
		}

		if (input->GetMouseButtonDown(MOUSEB_RIGHT)) {
			controls->clickDown(MOUSEB_RIGHT);
		} else {
			controls->release(MOUSEB_RIGHT);
		}
	}
}
