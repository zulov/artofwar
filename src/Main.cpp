#include <Urho3D/Engine/Application.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Engine/Console.h>
#include <Urho3D/UI/Cursor.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Engine/EngineDefs.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/UI/Sprite.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Core/Timer.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/UI/Window.h>
#include <Main.h>
#include <Urho3D/UI/Text3D.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Core/CoreEvents.h>

URHO3D_DEFINE_APPLICATION_MAIN(Main)


Main::Main(Context* context) : Application(context), useMouseMode_(MM_ABSOLUTE) {
	benchmark = new Benchmark();
}

void Main::Setup() {
	engineParameters_[EP_WINDOW_TITLE] = GetTypeName();
	engineParameters_[EP_LOG_NAME] = GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs") + GetTypeName() + ".log";
	engineParameters_[EP_FULL_SCREEN] = false;
	engineParameters_[EP_HEADLESS] = false;
	engineParameters_[EP_SOUND] = false;
	engineParameters_[EP_WINDOW_HEIGHT] = 768;
	engineParameters_[EP_WINDOW_WIDTH] = 1366;
}

void Main::Start() {
	SetWindowTitleAndIcon();

	SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(Main, HandleKeyDown));
	SubscribeToEvent(E_KEYUP, URHO3D_HANDLER(Main, HandleKeyUp));

	hud = new Hud(context_, GetSubsystem<UI>(), GetSubsystem<ResourceCache>(), GetSubsystem<Graphics>());
	hud->createStaticHud(String("Liczba jednostek") + String("??"));
	hud->createLogo();

	CreateConsoleAndDebugHud();

	levelBuilder = new LevelBuilder(GetSubsystem<ResourceCache>());
	scene = levelBuilder->CreateScene(context_);
	cameraManager = new CameraManager(context_);
	simulation = new Simulation(context_, GetSubsystem<ResourceCache>(), scene);
	simulation->createUnits();
	SetupViewport();
	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Main, HandleUpdate));

	InitMouseMode(MM_RELATIVE);
	controls = new Controls(GetSubsystem<UI>(), GetSubsystem<Graphics>());
}

void Main::Stop() {
	engine_->DumpResources(true);
}

void Main::HandleUpdate(StringHash eventType, VariantMap& eventData) {
	float timeStep = eventData[SceneUpdate::P_TIMESTEP].GetFloat();
	simulation->update(GetSubsystem<Input>(), timeStep);
	benchmark->add(1.0 / timeStep);
	hud->updateHud(benchmark, cameraManager);
	moveCamera(timeStep);
}

void Main::InitMouseMode(MouseMode mode) {
	useMouseMode_ = mode;

	Input* input = GetSubsystem<Input>();

	Console* console = GetSubsystem<Console>();
	if (useMouseMode_ != MM_ABSOLUTE) {
		input->SetMouseMode(useMouseMode_);
		if (console && console->IsVisible()) {
			input->SetMouseMode(MM_ABSOLUTE, true);
		}
	}

}

void Main::SetWindowTitleAndIcon() {
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	Graphics* graphics = GetSubsystem<Graphics>();
	Image* icon = cache->GetResource<Image>("textures/UrhoIcon.png");
	graphics->SetWindowIcon(icon);
	graphics->SetWindowTitle("Art of War 2017");
}

void Main::CreateConsoleAndDebugHud() {
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	XMLFile* xmlFile = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");

	hud->createConsole(engine_);
	hud->createDebugHud(engine_);

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

// If the user clicks the canvas, attempt to switch to relative mouse mode on web platform
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

	SharedPtr<Viewport> viewport(new Viewport(context_, scene, cameraManager->getComponent()));
	renderer->SetViewport(0, viewport);
}

void Main::clickLeft() {
	Vector3 hitPos;
	Drawable* hitDrawable;

	if (controls->raycast(250.0f, hitPos, hitDrawable, cameraManager->getComponent(), scene)) {
		Node* hitNode = hitDrawable->GetNode();

		if (hitNode->GetName() == "Box") {
			Node* child = hitNode->GetChild("title");
			Text3D* text = child->GetComponent<Text3D>();
			text->SetColor(Color::RED);

		} else if (hitNode->GetName() == "Ground") {

		}
	}
}

void Main::clickRight() {
	Vector3 hitPos;
	Drawable* hitDrawable;

	if (controls->raycast(250.0f, hitPos, hitDrawable, cameraManager->getComponent(), scene)) {
		Node* hitNode = hitDrawable->GetNode();

		if (hitNode->GetName() == "Box") {
			Node* child = hitNode->GetChild("title");
			Text3D* text = child->GetComponent<Text3D>();
			text->SetColor(Color::GREEN);

		} else if (hitNode->GetName() == "Ground") {

		}
	}
}

void Main::moveCamera(float timeStep) {
	if (GetSubsystem<UI>()->GetFocusElement()) { return; }

	Input* input = GetSubsystem<Input>();

	bool cameraKeys[4] = {input->GetKeyDown(KEY_W), input->GetKeyDown(KEY_S), input->GetKeyDown(KEY_A), input->GetKeyDown(KEY_D)};
	int wheel = input->GetMouseMoveWheel();
	cameraManager->translate(cameraKeys, wheel, timeStep);
	cameraManager->rotate(input->GetMouseMove());

	if (input->GetMouseButtonPress(MOUSEB_LEFT)) {
		clickLeft();
	} else if (input->GetMouseButtonPress(MOUSEB_RIGHT)) {
		clickRight();
	}
}

void Main::reset() {
	Input* input = GetSubsystem<Input>();

	if (input->GetKeyDown(KEY_P)) {
		resetUnits();
	}
}

void Main::resetUnits() {
	//	for (int i = 0; i < units->size(); i++) {
	//		scene->RemoveChild((*units)[i]->getNode());
	//		delete (*units)[i];
	//	}
	//	units->clear();
	//	createUnits(edgeSize, spaceSize);
}
