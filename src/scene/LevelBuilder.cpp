#include "LevelBuilder.h"
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Terrain.h>
#include <Urho3D/Graphics/Zone.h>
#include "database/DatabaseCache.h"
#include "database/db_other_struct.h"
#include "hud/window/main_menu/new_game/NewGameForm.h"
#include "load/dbload_container.h"
#include "load/SceneLoader.h"
#include "objects/NodeUtils.h"

LevelBuilder::LevelBuilder() {
	objectManager = new SceneObjectManager();
	scene = new Urho3D::Scene(Game::getContext());

	scene->CreateComponent<Urho3D::Octree>();
	Game::setScene(scene);
}

LevelBuilder::~LevelBuilder() {
	delete objectManager;
	scene->RemoveAllChildren(); //TODO tu czasme b³ad 
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
	if (!SIM_GLOBALS.HEADLESS) {
		objectManager->setZone(createNode("map/zone.xml"));
		objectManager->setLight(createNode("map/light.xml"));
	}
	objectManager->setGround(createGround(map->xmlName));
}

void LevelBuilder::createScene(NewGameForm* form) {
	createMap(form->map);
}

Urho3D::Terrain* LevelBuilder::getTerrain() const {
	return terrain;
}

Urho3D::Node* LevelBuilder::createGround(const Urho3D::String& xmlName) {
	auto node = createNode(xmlName);
	terrain = node->GetComponent<Urho3D::Terrain>();
	if (!SIM_GLOBALS.HEADLESS) {
		terrain->SetSmoothing(true);
		node->SetVar("ground", true);
	}

	return node;
}
