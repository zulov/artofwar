#include "LevelBuilder.h"

#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Terrain.h>
#include "database/DatabaseCache.h"
#include "database/db_other_struct.h"
#include "hud/window/main_menu/new_game/NewGameForm.h"
#include "load/dbload_container.h"
#include "load/SceneLoader.h"
#include "objects/NodeUtils.h"

LevelBuilder::LevelBuilder() {
	scene = new Urho3D::Scene(Game::getContext());

	scene->CreateComponent<Urho3D::Octree>();
	Game::setScene(scene);
}

LevelBuilder::~LevelBuilder() {
	scene->Remove();
	Game::setScene(nullptr);
}

void LevelBuilder::createScene(SceneLoader& loader) {
	loader.load();
	createMap(loader.getData()->config->map, loader.getConfig()->size / 256.f);
}

void LevelBuilder::createMap(int mapId, float spacing) {
	const auto map = Game::getDatabase()->getMaps()[mapId];
	if (!SIM_GLOBALS.HEADLESS) {
		createNode("map/zone.xml");
		createNode("map/light.xml");
	}
	createGround(map->xmlName, spacing);
}

void LevelBuilder::createScene(NewGameForm* form) {
	createMap(form->map, form->size / 256.f);
}

Urho3D::Terrain* LevelBuilder::getTerrain() const {
	return terrain;
}

void LevelBuilder::createGround(const Urho3D::String& xmlName, float spacing) {
	if (!SIM_GLOBALS.FAKE_TERRAIN) {
		auto node = createNode(xmlName);
		terrain = node->GetComponent<Urho3D::Terrain>();
		auto s = terrain->GetSpacing();
		s.x_ = spacing;
		s.z_ = spacing;
		terrain->SetSpacing(s);

		terrain->GetMaterial()->SetUVTransform(Urho3D::Vector2(0, 0), 0.f, 16.f * spacing);

		if (!SIM_GLOBALS.HEADLESS) {
			terrain->SetSmoothing(true);
			node->SetVar("ground", true);
		}
	}
}
