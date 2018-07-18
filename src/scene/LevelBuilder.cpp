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
	scene = new Urho3D::Scene(Game::getContext());

	scene->CreateComponent<Urho3D::Octree>();

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
	Entity* light = createLight(Urho3D::Vector3(0.6f, -1.0f, 0.8f), Urho3D::Color(0.7f, 0.6f, 0.6f),
	                            Urho3D::LIGHT_DIRECTIONAL);
	Entity* ground = createGround(map->height_map, map->texture, map->scale_hor, map->scale_ver);

	objectManager->add(zone);
	objectManager->add(light);
	objectManager->add(ground);
}

void LevelBuilder::createScene(NewGameForm* form) {
	createMap(form->map);
}

Urho3D::Terrain* LevelBuilder::getTerrian() {
	return terrain;
}

Entity* LevelBuilder::createZone() {
	Entity* entity = new Entity(ObjectType::ENTITY);

	auto zoneNode = entity->getNode();
	auto zone = zoneNode->CreateComponent<Urho3D::Zone>();
	zone->SetBoundingBox(Urho3D::BoundingBox(-1000.0f, 1000.0f));
	zone->SetFogColor(Urho3D::Color(0.15f, 0.15f, 0.3f));
	zone->SetFogStart(200);
	zone->SetFogEnd(300);

	return entity;
}

Entity* LevelBuilder::createLight(const Urho3D::Vector3& direction, const Urho3D::Color& color,
                                  Urho3D::LightType lightType) {
	Entity* entity = new Entity(ObjectType::ENTITY);
	auto lightNode = entity->getNode();
	lightNode->SetDirection(direction);
	auto light = lightNode->CreateComponent<Urho3D::Light>();
	light->SetPerVertex(true);
	light->SetLightType(lightType);
	light->SetColor(color);

	return entity;
}

Entity* LevelBuilder::createGround(const Urho3D::String& heightMap, const Urho3D::String& texture, float horScale,
                                   float verScale) {
	Entity* entity = new Physical(new Urho3D::Vector3, ObjectType::PHYSICAL);

	auto node = entity->getNode();

	terrain = node->CreateComponent<Urho3D::Terrain>();
	terrain->SetPatchSize(8);
	terrain->SetSpacing(Urho3D::Vector3(horScale, verScale, horScale));
	terrain->SetSmoothing(false);
	terrain->SetOccluder(true);
	terrain->SetHeightMap(Game::getCache()->GetResource<Urho3D::Image>(heightMap));
	auto mat = Game::getCache()->GetResource<Urho3D::Material>(texture)->Clone();
	mat->SetRenderOrder(100); // Lower render order to render first
	auto tecs = mat->GetTechniques();
	for (size_t i = 0; i < tecs.Size(); ++i) {
		auto tec = mat->GetTechnique(i)->Clone();
		auto passes = tec->GetPasses();
		for (auto pass : passes) {
			pass->SetDepthWrite(false);
		}
		mat->SetTechnique(i, tec);
	}
	terrain->SetMaterial(mat);
	return entity;
}
