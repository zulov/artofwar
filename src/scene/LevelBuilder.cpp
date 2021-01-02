#include "LevelBuilder.h"
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Technique.h>
#include <Urho3D/Graphics/Terrain.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Resource/Image.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include "Game.h"
#include "database/DatabaseCache.h"
#include "database/db_other_struct.h"
#include "hud/window/main_menu/new_game/NewGameForm.h"
#include "load/dbload_container.h"
#include "load/SceneLoader.h"
#include "objects/Physical.h"

LevelBuilder::LevelBuilder() {
	objectManager = new SceneObjectManager();
	scene = new Urho3D::Scene(Game::getContext());

	scene->CreateComponent<Urho3D::Octree>();
	Game::setScene(scene);
}

LevelBuilder::~LevelBuilder() {
	delete objectManager;
	scene->RemoveAllChildren();//TODO tu czasme b³ad 
	scene->Clear();
	scene->Remove();
	Game::setScene(nullptr);
}

void LevelBuilder::createScene(SceneLoader& loader) {
	loader.load();

	createMap(loader.getData()->config->map);
}

void LevelBuilder::createMap(int mapId) {
	const auto map = Game::getDatabase()->getMaps()[mapId];

	objectManager->setZone(createZone());
	objectManager->setLight(createLight({0.6f, -1.0f, 0.8f}, {0.7f, 0.6f, 0.6f},
	                               Urho3D::LIGHT_DIRECTIONAL));
	objectManager->setGround(createGround(map->height_map, map->texture, map->scale_hor, map->scale_ver));
}

void LevelBuilder::createScene(NewGameForm* form) {
	createMap(form->map);
}

Urho3D::Terrain* LevelBuilder::getTerrain() const {
	return terrain;
}

Urho3D::Node* LevelBuilder::createZone() {
	auto zoneNode = Game::getScene()->CreateChild();
	auto zone = zoneNode->CreateComponent<Urho3D::Zone>();
	zone->SetBoundingBox(Urho3D::BoundingBox(-1000.0f, 1000.0f));
	zone->SetFogColor(Urho3D::Color(0.15f, 0.15f, 0.3f));
	zone->SetFogStart(200);
	zone->SetFogEnd(300);

	return zoneNode;
}

Urho3D::Node* LevelBuilder::createLight(const Urho3D::Vector3& direction, const Urho3D::Color& color,
                                  Urho3D::LightType lightType) {
	auto lightNode = Game::getScene()->CreateChild();
	lightNode->SetDirection(direction);
	auto light = lightNode->CreateComponent<Urho3D::Light>();
	light->SetPerVertex(true);
	light->SetLightType(lightType);
	light->SetColor(color);

	light->SetCastShadows(true);
	light->SetShadowBias(Urho3D::BiasParameters(0.00025f, 0.5f));
    light->SetShadowCascade(Urho3D::CascadeParameters(10.0f, 50.0f, 200.0f, 0.0f, 0.8f));
    light->SetSpecularIntensity(0.5f);
	light->SetPerVertex(false);
	
	return lightNode;
}

Physical* LevelBuilder::createGround(const Urho3D::String& heightMap, const Urho3D::String& texture,
                                   float horScale, float verScale) {
	auto pos = Urho3D::Vector3();
	const auto entity = new Physical(pos);

	terrain = entity->getNode()->CreateComponent<Urho3D::Terrain>();
	terrain->SetHeightMap(Game::getCache()->GetResource<Urho3D::Image>(heightMap));
	terrain->SetPatchSize(8);
	terrain->SetSpacing({horScale, verScale, horScale});
	terrain->SetSmoothing(true);
	terrain->SetOccluder(false);
	terrain->SetMaterial(Game::getCache()->GetResource<Urho3D::Material>(texture));
	return entity;
}
