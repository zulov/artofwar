#include <Main.h>

URHO3D_DEFINE_APPLICATION_MAIN(Main)


Main::Main(Context* context) : Application(context), useMouseMode_(MM_ABSOLUTE) {
	benchmark = new Benchmark();
	context->RegisterFactory<LinkComponent>();
}

void Main::Setup() {
	engineParameters_[EP_WINDOW_TITLE] = GetTypeName();
	engineParameters_[EP_LOG_NAME] = GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs") + GetTypeName() + ".log";
	engineParameters_[EP_FULL_SCREEN] = false;
	engineParameters_[EP_HEADLESS] = false;
	engineParameters_[EP_SOUND] = false;
	engineParameters_[EP_WINDOW_HEIGHT] = 768;
	engineParameters_[EP_WINDOW_WIDTH] = 1366;
	engine_->SetMaxFps(10000);
	engine_->SetMinFps(0.1);
}

void Main::Start() {
	Game* game = Game::get();
	game->setCache(GetSubsystem<ResourceCache>())->setUI(GetSubsystem<UI>())->setGraphics(GetSubsystem<Graphics>())->setConsole(GetSubsystem<Console>())->setContext(context_)->setEngine(engine_);
	SetWindowTitleAndIcon();
	SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(Main, HandleKeyDown));
	SubscribeToEvent(E_KEYUP, URHO3D_HANDLER(Main, HandleKeyUp));

	hud = new Hud();
	hud->createStaticHud(String("Liczba jednostek") + String("??"));
	//hud->createLogo();

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
	game->setScene(levelBuilder->createScene())->setCameraManager(cameraManager)->setBuildList(buildList)->setSimCommandList(simulationCommandList)->setMediator(mediator)->setDatabaseCache(new DatabaseCache());

	simulation = new Simulation(enviromentStrategy, simulationCommandList, simulationObjectManager);
	//simulation->createUnits();
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
	levelBuilder->execute();
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

void Main::SetupViewport() {
	Renderer* renderer = GetSubsystem<Renderer>();

	SharedPtr<Viewport> viewport(new Viewport(context_, Game::get()->getScene(), cameraManager->getComponent()));
	renderer->SetViewport(0, viewport);
}

void Main::control(float timeStep) {
	if (GetSubsystem<UI>()->GetFocusElement()) { return; }
	controls->updateState();
	Input* input = GetSubsystem<Input>();

	bool cameraKeys[4] = {input->GetKeyDown(KEY_W), input->GetKeyDown(KEY_S), input->GetKeyDown(KEY_A), input->GetKeyDown(KEY_D)};
	int wheel = input->GetMouseMoveWheel();
	cameraManager->translate(cameraKeys, wheel, timeStep);
	cameraManager->rotate(input->GetMouseMove());

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
