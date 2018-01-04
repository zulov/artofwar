#include "LevelBuilder.h"
#include "Game.h"
#include "Urho3D/Resource/Image.h"
#include "database/DatabaseCache.h"
#include "objects/Physical.h"
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Terrain.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Resource/ResourceCache.h>


LevelBuilder::LevelBuilder() {
	objectManager = new SceneObjectManager();
	scene = new Scene(Game::get()->getContext());

	scene->CreateComponent<Octree>();
	Game::get()->setScene(scene);
}

LevelBuilder::~LevelBuilder() {
	delete objectManager;
	scene->RemoveAllChildren();
	scene->Clear();
	scene->Remove();
	Game::get()->setScene(nullptr);
}

void LevelBuilder::createScene(SceneLoader& loader) {
	loader.load();
	dbload_container* data = loader.getData();

	db_map* map = Game::get()->getDatabaseCache()->getMap(data->config->map);
	Entity* zone = createZone();
	Entity* light = createLight(Vector3(0.6f, -1.0f, 0.8f), Color(0.7f, 0.6f, 0.6f), LIGHT_DIRECTIONAL);
	Entity* ground = createGround(map->height_map, map->texture, map->scale_hor, map->scale_ver);

	objectManager->add(zone);
	objectManager->add(light);
	objectManager->add(ground);
}

Terrain* LevelBuilder::getTerrian() {
	return terrain;
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

Entity* LevelBuilder::createLight(Vector3 direction, Color color, LightType lightType) {
	Entity* entity = new Entity(ENTITY);
	Node* lightNode = entity->getNode();
	lightNode->SetDirection(direction);
	Light* light = lightNode->CreateComponent<Light>();
	light->SetPerVertex(true);
	light->SetLightType(lightType);
	light->SetColor(color);

	return entity;
}

Entity* LevelBuilder::createGround(String heightMap, String texture, float horScale, float verScale) {
	Entity* entity = new Physical(new Vector3, PHISICAL);

	Node* node = entity->getNode();

	terrain = node->CreateComponent<Terrain>();
	terrain->SetPatchSize(8);
	terrain->SetSpacing(Vector3(horScale, verScale, horScale));
	terrain->SetSmoothing(false);
	terrain->SetOccluder(true);
	terrain->SetHeightMap(Game::get()->getCache()->GetResource<Image>(heightMap));
	terrain->SetMaterial(Game::get()->getCache()->GetResource<Material>(texture));

	return entity;
}
