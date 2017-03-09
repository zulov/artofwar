#include "LevelBuilder.h"


LevelBuilder::LevelBuilder(ResourceCache* _cache) {
	cache = _cache;
}


LevelBuilder::~LevelBuilder() {}

SharedPtr<Scene> LevelBuilder::CreateScene(Context *context) {
	if (!scene) {
		scene = new Scene(context);
	} else {
		scene->Clear();
	}
	scene->CreateComponent<Octree>();

	createZone();
	createLight();
	createGround();
	return scene;
}


void LevelBuilder::createZone() {
	Node* zoneNode = scene->CreateChild("Zone");
	Zone* zone = zoneNode->CreateComponent<Zone>();
	zone->SetBoundingBox(BoundingBox(-1000.0f, 1000.0f));
	zone->SetFogColor(Color(0.2f, 0.2f, 0.2f));
	zone->SetFogStart(200.0f);
	zone->SetFogEnd(300.0f);
}

void LevelBuilder::createLight() {
	Node* lightNode = scene->CreateChild("DirectionalLight");
	lightNode->SetDirection(Vector3(-0.6f, -1.0f, -0.8f)); // The direction vector does not need to be normalized
	Light* light = lightNode->CreateComponent<Light>();
	light->SetLightType(LIGHT_DIRECTIONAL);
	light->SetColor(Color(0.7f, 0.35f, 0.0f));
}

void LevelBuilder::createGround() {
	Node* planeNode = scene->CreateChild("Ground");
	planeNode->SetScale(Vector3(300, 1.0f, 300));
	planeNode->SetPosition(Vector3(0, -1.0f, 0));
	StaticModel* planeObject = planeNode->CreateComponent<StaticModel>();
	planeObject->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));
	planeObject->SetMaterial(cache->GetResource<Material>("Materials/StoneTiled.xml"));
}