#include "LevelBuilder.h"
#include "objects/Physical.h"


LevelBuilder::LevelBuilder(SceneObjectManager* _objectManager) {
	objectManager = _objectManager;
	scene = new Scene(Game::get()->getContext());

	scene->CreateComponent<Octree>();
	Game::get()->setScene(scene);
	mapReader = new MapReader();
	Model* model = mapReader->read("data/map/mini.png");

	Node* node = Game::get()->getScene()->CreateChild("FromScratchObject");
	node->SetPosition(Vector3(-0.0f, 20.0f, 0.0f));
	
	StaticModel* object = node->CreateComponent<StaticModel>();

	object->SetModel(model);
	object->SetMaterial(Game::get()->getCache()->GetResource<Material>("Materials/red.xml"));
	//mapReader->read("data/map/test2.png");
	//mapReader->read("data/map/test3.png");
}

LevelBuilder::~LevelBuilder() {
	delete mapReader;
}

void LevelBuilder::createScene() {
	Entity* zone = createZone();
	Entity* light = createLight();
	Entity* ground = createGround();

	objectManager->add(zone);
	objectManager->add(light);
	objectManager->add(ground);
}

void LevelBuilder::execute() {
}

Entity* LevelBuilder::createZone() {
	Entity* entity = new Entity(ENTITY);

	Node* zoneNode = entity->getNode();
	Zone* zone = zoneNode->CreateComponent<Zone>();
	zone->SetBoundingBox(BoundingBox(-1000.0f, 1000.0f));
	zone->SetFogColor(Color(0.15f, 0.15f, 0.3f));
	zone->SetFogStart(200);
	zone->SetFogEnd(300);

	return entity;
}

Entity* LevelBuilder::createLight() {
	Entity* entity = new Entity(ENTITY);
	Node* lightNode = entity->getNode();
	lightNode->SetDirection(Vector3(0.6f, -1.0f, 0.8f));
	Light* light = lightNode->CreateComponent<Light>();
	light->SetLightType(LIGHT_DIRECTIONAL);
	light->SetColor(Color(0.7f, 0.6f, 0.6f));

	return entity;
}

Entity* LevelBuilder::createGround() {
	Entity* entity = new Physical(new Vector3, PHISICAL);

	Node* planeNode = entity->getNode();
	planeNode->SetScale(Vector3(1024, 1.0f, 1024));

	StaticModel* planeObject = planeNode->CreateComponent<StaticModel>();
	planeObject->SetModel(Game::get()->getCache()->GetResource<Model>("Models/Plane.mdl"));
	planeObject->SetMaterial(Game::get()->getCache()->GetResource<Material>("Materials/test.xml"));

	return entity;
}
