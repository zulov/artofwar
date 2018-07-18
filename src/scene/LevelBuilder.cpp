#include "LevelBuilder.h"
#include "Game.h"
#include "ObjectEnums.h"
#include "Urho3D/Resource/Image.h"
#include "database/DatabaseCache.h"
#include "hud/window/main_menu/new_game/NewGameForm.h"
#include "load/SceneLoader.h"
#include "objects/Physical.h"
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Technique.h>
#include <Urho3D/Graphics/Terrain.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Resource/ResourceCache.h>

LevelBuilder::LevelBuilder() {
	objectManager = new SceneObjectManager();
	scene = new Scene(Game::getContext());

	scene->CreateComponent<Octree>();

	Game::setScene(scene);
}

LevelBuilder::~LevelBuilder() {
	delete objectManager;
	scene->RemoveAllChildren();
	scene->Clear();
	scene->Remove();
	Game::setScene(nullptr);
}

void LevelBuilder::createScene(SceneLoader& loader) {
	loader.load();
	dbload_container* data = loader.getData();

	createMap(data->config->map);
}

void LevelBuilder::createMap(int mapId) {
	db_map* map = Game::getDatabaseCache()->getMap(mapId);
	Entity* zone = createZone();
	Entity* light = createLight(Vector3(0.6f, -1.0f, 0.8f), Color(0.7f, 0.6f, 0.6f), LIGHT_DIRECTIONAL);
	Entity* ground = createGround(map->height_map, map->texture, map->scale_hor, map->scale_ver);

	objectManager->add(zone);
	objectManager->add(light);
	objectManager->add(ground);
}

void LevelBuilder::createScene(NewGameForm* form) {
	createMap(form->map);
}

Terrain* LevelBuilder::getTerrian() {
	return terrain;
}

Entity* LevelBuilder::createZone() {
	Entity* entity = new Entity(ObjectType::ENTITY);

	Node* zoneNode = entity->getNode();
	Zone* zone = zoneNode->CreateComponent<Zone>();
	zone->SetBoundingBox(BoundingBox(-1000.0f, 1000.0f));
	zone->SetFogColor(Color(0.15f, 0.15f, 0.3f));
	zone->SetFogStart(200);
	zone->SetFogEnd(300);

	return entity;
}

Entity* LevelBuilder::createLight(const Vector3& direction, const Color& color, LightType lightType) {
	Entity* entity = new Entity(ObjectType::ENTITY);
	Node* lightNode = entity->getNode();
	lightNode->SetDirection(direction);
	Light* light = lightNode->CreateComponent<Light>();
	light->SetPerVertex(true);
	light->SetLightType(lightType);
	light->SetColor(color);

	return entity;
}

Entity* LevelBuilder::createGround(const String& heightMap, const String& texture, float horScale, float verScale) {
	Entity* entity = new Physical(new Vector3, ObjectType::PHYSICAL);

	Node* node = entity->getNode();

	terrain = node->CreateComponent<Terrain>();
	terrain->SetPatchSize(8);
	terrain->SetSpacing(Vector3(horScale, verScale, horScale));
	terrain->SetSmoothing(false);
	terrain->SetOccluder(true);
	terrain->SetHeightMap(Game::getCache()->GetResource<Image>(heightMap));
	auto mat = Game::getCache()->GetResource<Material>(texture)->Clone();
	mat->SetRenderOrder(100); // Lower render order to render first
	auto tecs = mat->GetTechniques();
	for (size_t i = 0; i < tecs.Size(); ++i) {
		auto tec = mat->GetTechnique(i)->Clone();
		auto passes = tec->GetPasses();
		for (auto pass : passes)
			pass->SetDepthWrite(false);
		mat->SetTechnique(i, tec);
	}
	terrain->SetMaterial(mat);
	return entity;
}
