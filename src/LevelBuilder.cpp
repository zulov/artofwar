#include "LevelBuilder.h"
#include "ObjectManager.h"
#include "Game.h"


LevelBuilder::LevelBuilder() {
}


LevelBuilder::~LevelBuilder() {
}

SharedPtr<Scene> LevelBuilder::CreateScene(ObjectManager* objectManager) {
	if (!scene) {
		scene = new Scene(Game::getInstance()->getContext());
	} else {
		scene->Clear();
	}
	scene->CreateComponent<Octree>();

	Entity* zone = createZone();
	Entity* light = createLight();
	Entity* ground = createGround();

	//objectManager.add(zone);
	//objectManager.add(light);
	//objectManager.add(ground);

	return scene;
}


Entity* LevelBuilder::createZone() {
	Node* zoneNode = scene->CreateChild("Zone");
	Zone* zone = zoneNode->CreateComponent<Zone>();
	zone->SetBoundingBox(BoundingBox(-1000.0f, 1000.0f));
	zone->SetFogColor(Color(0.2f, 0.2f, 0.2f));
	zone->SetFogStart(200.0f);
	zone->SetFogEnd(300.0f);

	Entity * entity = new Entity(new Vector3(), zoneNode, nullptr);
	return entity;
}

Entity* LevelBuilder::createLight() {
	Node* lightNode = scene->CreateChild("DirectionalLight");
	lightNode->SetDirection(Vector3(-0.6f, -1.0f, -0.8f)); // The direction vector does not need to be normalized
	Light* light = lightNode->CreateComponent<Light>();
	light->SetLightType(LIGHT_DIRECTIONAL);
	light->SetColor(Color(0.7f, 0.35f, 0.0f));

	Entity * entity = new Entity(new Vector3(), lightNode, nullptr);
	return entity;
}

Entity* LevelBuilder::createGround() {
	Node* planeNode = scene->CreateChild("Ground");
	planeNode->SetScale(Vector3(300, 1.0f, 300));
	planeNode->SetPosition(Vector3(0, -1.0f, 0));
	StaticModel* planeObject = planeNode->CreateComponent<StaticModel>();
	planeObject->SetModel(Game::getInstance()->getCache()->GetResource<Model>("Models/Plane.mdl"));
	planeObject->SetMaterial(Game::getInstance()->getCache()->GetResource<Material>("Materials/StoneTiled.xml"));

	Entity * entity = new Entity(new Vector3(), planeNode, nullptr);
	return entity;

}
