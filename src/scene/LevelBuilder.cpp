#include "LevelBuilder.h"

#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Terrain.h>
#include <Urho3D/Graphics/TerrainPatch.h>
#include <Urho3D/Graphics/Geometry.h>
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
	createMap(loader.getData()->config->map, loader.getConfig()->size);
}

void LevelBuilder::createMap(int mapId, int size) {
	const auto map = Game::getDatabase()->getMaps()[mapId];
	if (!SIM_GLOBALS.HEADLESS) {
		createNode("map/zone.xml");
		createNode("map/light.xml");
	}
	createGround(map->xmlName, size);
}

void LevelBuilder::createScene(NewGameForm* form) {
	createMap(form->map, form->size / 256.f);
}

Urho3D::Terrain* LevelBuilder::getTerrain() const {
	return terrain;
}

void LevelBuilder::createGround(const Urho3D::String& xmlName, int size) {
	if (!SIM_GLOBALS.FAKE_TERRAIN) {
		const auto hSpacing = size / 256.f;
		auto node = createNode(xmlName);
		terrain = node->GetComponent<Urho3D::Terrain>();
		auto s = terrain->GetSpacing();
		s.x_ = hSpacing;
		s.y_ *= hSpacing;
		s.z_ = hSpacing;
		terrain->SetSpacing(s);

		terrain->GetMaterial()->SetUVTransform(Urho3D::Vector2(0, 0), 0.f, 16.f * hSpacing);

		darkMat = terrain->GetMaterial()->Clone();
		darkMat->SetShaderParameter("MatDiffColor", Urho3D::Color(0.5, 0.5, 0.5, 1));

		extraDarkMat = terrain->GetMaterial()->Clone();
		extraDarkMat->SetShaderParameter("MatDiffColor", Urho3D::Color(0.2, 0.2, 0.2, 2));
		
		if (!SIM_GLOBALS.HEADLESS) {
			terrain->SetSmoothing(true);
			node->SetVar("ground", true);
		}
	}
}
