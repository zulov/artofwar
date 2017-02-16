#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Core/Profiler.h>
#include <Urho3D/Engine/Engine.h>

#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/StaticModelGroup.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/DebugNew.h>

#include "Simulation.h"

URHO3D_DEFINE_APPLICATION_MAIN(Simulation)

Simulation::Simulation(Context* context) :Main(context), animate(false) {
	benchmark = new Benchmark();
	units = new std::vector<Unit *>();
	units->reserve(edgeSize*edgeSize);
	
}

void Simulation::Start() {
	Main::Start();
	CreateScene();

	SetupViewport();
	SubscribeToEvents();
	Main::InitMouseMode(MM_RELATIVE);
	srand(time(NULL));
	envStrategy = new EnviromentStrategy();
	forceStrategy = new ForceStrategy();
	envStrategy->prepare(units);
	hud = new Hud(context_, GetSubsystem<UI>(), GetSubsystem<ResourceCache>(), GetSubsystem<Graphics>());
	hud->createStaticHud(String("Liczba jednostek") + String(units->size()));
	controls = new Controls(GetSubsystem<UI>(), GetSubsystem<Graphics>());
	createGround();

}

void Simulation::createGround() {
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	Node* planeNode = scene->CreateChild("Ground");
	planeNode->SetScale(Vector3(100.0f, 1.0f, 100.0f));
	planeNode->SetPosition(Vector3(0, -1.0f, 0));
	StaticModel* planeObject = planeNode->CreateComponent<StaticModel>();
	planeObject->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));
	planeObject->SetMaterial(cache->GetResource<Material>("Materials/StoneTiled.xml"));
}

void Simulation::CreateScene() {
	if (!scene) {
		scene = new Scene(context_);
	} else {
		scene->Clear();
	}
	scene->CreateComponent<Octree>();

	createZone();
	createLight();
	createUnits(edgeSize, spaceSize);
	createCamera();
}


void Simulation::createCamera() {
	cameraManager = new CameraManager(context_);
}

void Simulation::createLight() {
	Node* lightNode = scene->CreateChild("DirectionalLight");
	lightNode->SetDirection(Vector3(-0.6f, -1.0f, -0.8f)); // The direction vector does not need to be normalized
	Light* light = lightNode->CreateComponent<Light>();
	light->SetLightType(LIGHT_DIRECTIONAL);
	light->SetColor(Color(0.7f, 0.35f, 0.0f));

}


void Simulation::SubscribeToEvents() {
	// Subscribe HandleUpdate() function for processing update events
	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Simulation, HandleUpdate));
}

void Simulation::moveCamera(float timeStep) {
	// Do not move if the UI has a focused element (the console)
	if (GetSubsystem<UI>()->GetFocusElement()) { return; }

	Input* input = GetSubsystem<Input>();

	const float MOVE_SPEED = 20.0f;
	const float MOUSE_SENSITIVITY = 0.1f;

	// Use this frame's mouse motion to adjust camera node yaw and pitch. Clamp the pitch between -90 and 90 degrees
	IntVector2 mouseMove = input->GetMouseMove();
	yaw_ += MOUSE_SENSITIVITY * mouseMove.x_;
	pitch_ += MOUSE_SENSITIVITY * mouseMove.y_;
	//pitch_ = Clamp(pitch_, -90.0f, 90.0f);

	cameraManager->setRotation(Quaternion(pitch_, yaw_, 0.0f));

	bool cameraKeys[4] = { input->GetKeyDown(KEY_W) ,input->GetKeyDown(KEY_S), input->GetKeyDown(KEY_A), input->GetKeyDown(KEY_D) };
	int wheel = input->GetMouseMoveWheel();
	cameraManager->translate(cameraKeys, wheel, timeStep);

	// Set destination or spawn a new jack with left mouse button
	if (input->GetMouseButtonPress(MOUSEB_LEFT)) {
		clickLeft();
	} else if (input->GetMouseButtonPress(MOUSEB_RIGHT)) {
		clickRight();
	}

}

void Simulation::AnimateObjects(float timeStep) {
	URHO3D_PROFILE(AnimateObjects);
	calculateForces();
	moveUnits(timeStep);
}

void Simulation::moveUnits(float timeStep) {
	for (unsigned i = 0; i < units->size(); ++i) {
		units->at(i)->applyForce(timeStep);
		units->at(i)->move(timeStep);
		envStrategy->update(units->at(i));
	}
}

void Simulation::HandleUpdate(StringHash eventType, VariantMap& eventData) {
	using namespace Update;

	float timeStep = eventData[P_TIMESTEP].GetFloat();

	Input* input = GetSubsystem<Input>();
	if (input->GetKeyPress(KEY_SPACE)) {
		animate = !animate;
	}

	reset();
	moveCamera(timeStep);

	// Animate scene if enabled
	if (animate) {
		AnimateObjects(timeStep);
	}
	updateHud(timeStep);
}

void Simulation::calculateForces() {
	for (unsigned i = 0; i < units->size(); ++i) {
		std::vector<Unit*> *neighbours = envStrategy->getNeighbours((*units)[i], units);

		Vector3  sepPedestrian = forceStrategy->separationUnits((*units)[i], neighbours);
		Vector3  sepObstacle = forceStrategy->separationObstacle((*units)[i], 0);

		Vector3  destForce = Vector3();// forceStrategy->destinationForce((*units)[i]);
		Vector3  rand = forceStrategy->randomForce();

		Vector3  forces = sepPedestrian += sepObstacle += destForce += rand;

		(*units)[i]->setAcceleration(forces);

		delete neighbours;
	}
}

void Simulation::reset() {
	Input* input = GetSubsystem<Input>();

	if (input->GetKeyDown(KEY_P)) {
		resetUnits();
	}
}

void Simulation::resetUnits() {
	for (int i = 0; i < units->size(); i++) {
		scene->RemoveChild((*units)[i]->getNode());
		delete (*units)[i];
	}
	units->clear();
	createUnits(edgeSize, spaceSize);
}

void Simulation::updateHud(float timeStep) {
	FrameInfo frameInfo = GetSubsystem<Renderer>()->GetFrameInfo();
	double fps = 1.0 / timeStep;

	benchmark->add(fps);
	Urho3D::String msg = "Frames: " + String(frameInfo.frameNumber_);
	msg += "\nFPS: " + String(fps);
	msg += "\navg FPS: " + String(benchmark->getAverageFPS());
	msg += "\nCamera: ";
	msg += "\n\t" + cameraManager->getInfo();

	ResourceCache* cache = GetSubsystem<ResourceCache>();
	UI* ui = GetSubsystem<UI>();
	ui->GetRoot()->RemoveChild(fpsText);

	fpsText = ui->GetRoot()->CreateChild<Text>();
	fpsText->SetText(msg);
	fpsText->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 12);
	fpsText->SetTextAlignment(HA_LEFT);
	fpsText->SetHorizontalAlignment(HA_LEFT);
	fpsText->SetVerticalAlignment(VA_TOP);
	fpsText->SetPosition(0, 20);

}


void Simulation::createUnits(int size, double space) {
	int startSize = -(size / 2);
	int endSize = size + startSize;

	ResourceCache* cache = GetSubsystem<ResourceCache>();

	for (int y = startSize; y < endSize; ++y) {
		for (int x = startSize; x < endSize; ++x) {
			Vector3 position = Vector3(x * space, 0, y * space);
			Node* boxNode = scene->CreateChild("Box");
			boxNode->SetPosition(position);

			StaticModel* boxObject = boxNode->CreateComponent<StaticModel>();
			boxObject->SetModel(cache->GetResource<Model>("Models/Cube.mdl"));

			Node * title = boxNode->CreateChild("title");
			title->SetPosition(Vector3(0.0f, 1.2f, 0.0f));
			Text3D* titleText = title->CreateComponent<Text3D>();
			titleText->SetText("Entity");

			titleText->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 24);
			titleText->SetColor(Color::GREEN);
			titleText->SetAlignment(HA_CENTER, VA_CENTER);
			titleText->SetFaceCameraMode(FC_LOOKAT_MIXED);

			Unit * newUnit = new Unit(position, boxNode);
			units->push_back(newUnit);
		}
	}

}

void Simulation::createZone() {
	Node* zoneNode = scene->CreateChild("Zone");
	Zone* zone = zoneNode->CreateComponent<Zone>();
	zone->SetBoundingBox(BoundingBox(-1000.0f, 1000.0f));
	zone->SetFogColor(Color(0.2f, 0.2f, 0.2f));
	zone->SetFogStart(200.0f);
	zone->SetFogEnd(300.0f);
}

void Simulation::clickLeft() {
	Vector3 hitPos;
	Drawable* hitDrawable;

	if (controls->raycast(250.0f, hitPos, hitDrawable, cameraManager->getComponent(), scene)) {
		Node* hitNode = hitDrawable->GetNode();

		if (hitNode->GetName() == "Box") {
			Node* child = hitNode->GetChild("title");
			Text3D * text = child->GetComponent<Text3D>();
			text->SetColor(Color::RED);
			
		} else if (hitNode->GetName() == "Ground") {
			
		}
	}
}


void Simulation::clickRight() {
	Vector3 hitPos;
	Drawable* hitDrawable;

	if (controls->raycast(250.0f, hitPos, hitDrawable, cameraManager->getComponent(), scene)) {
		Node* hitNode = hitDrawable->GetNode();

		if (hitNode->GetName() == "Box") {
			Node* child = hitNode->GetChild("title");
			Text3D * text = child->GetComponent<Text3D>();
			text->SetColor(Color::GREEN);
		
		} else if (hitNode->GetName() == "Ground") {

		}
	}
}