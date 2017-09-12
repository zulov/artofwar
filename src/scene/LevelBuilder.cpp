#include "LevelBuilder.h"
#include "objects/Physical.h"


LevelBuilder::LevelBuilder(SceneObjectManager* _objectManager) {
	objectManager = _objectManager;
}

LevelBuilder::~LevelBuilder() {

}

SharedPtr<Scene> LevelBuilder::createScene() {
	scene = new Scene(Game::get()->getContext());

	scene->CreateComponent<Octree>();

	Entity* zone = createZone();
	Entity* light = createLight();
	Entity* ground = createGround();

	objectManager->add(zone);
	objectManager->add(light);
	objectManager->add(ground);

	return scene;
}

void LevelBuilder::execute() {
	//buildList->execute();
}

Entity* LevelBuilder::createZone() {
	Node* zoneNode = scene->CreateChild();
	Zone* zone = zoneNode->CreateComponent<Zone>();
	zone->SetBoundingBox(BoundingBox(-1000.0f, 1000.0f));
	zone->SetFogColor(Color(0.15f, 0.15f, 0.3f));
	zone->SetFogStart(200.0f);
	zone->SetFogEnd(300.0f);

	Entity* entity = new Entity(zoneNode, ENTITY);
	return entity;
}

Entity* LevelBuilder::createLight() {
	Node* lightNode = scene->CreateChild();
	lightNode->SetDirection(Vector3(0.6f, -1.0f, 0.8f)); 
	Light* light = lightNode->CreateComponent<Light>();
	light->SetLightType(LIGHT_DIRECTIONAL);
	light->SetColor(Color(0.7f, 0.6f, 0.6f));

	Entity* entity = new Entity(lightNode, ENTITY);
	return entity;
}

Entity* LevelBuilder::createGround() {
	Node* planeNode = scene->CreateChild();
	planeNode->SetScale(Vector3(300, 1.0f, 300));
	planeNode->SetPosition(Vector3());
	StaticModel* planeObject = planeNode->CreateComponent<StaticModel>();
	planeObject->SetModel(Game::get()->getCache()->GetResource<Model>("Models/Plane.mdl"));
	planeObject->SetMaterial(Game::get()->getCache()->GetResource<Material>("Materials/StoneTiled.xml"));

	Entity* entity = new Physical(new Vector3, planeNode, PHISICAL);
	return entity;

}
