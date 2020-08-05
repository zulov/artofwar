#include "BillboardSetProvider.h"


#include <Urho3D/Graphics/BillboardSet.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/ResourceCache.h>
#include "Game.h"
#include "database/DatabaseCache.h"
#include "objects/ObjectEnums.h"
#include "player/Player.h"
#include "player/PlayersManager.h"

Urho3D::BillboardSet* BillboardSetProvider::createSet(Urho3D::String& materialName) const {
	auto bs = node->CreateComponent<Urho3D::BillboardSet>();
	bs->SetNumBillboards(1000);

	bs->SetMaterial(Game::getCache()->GetResource<Urho3D::Material>(materialName));
	bs->SetSorted(true);
	return bs;
}

BillboardSetProvider::BillboardSetProvider() {
	node = Game::getScene()->CreateChild();

	for (auto player : Game::getPlayersMan()->getAllPlayers()) {
		auto material = "Materials/select/select_" + Game::getDatabase()->getPlayerColor(player->getId())->name +
			".xml";
		perPlayerAura.push_back(createSet(material));
		material = "Materials/bar/bar_" + Game::getDatabase()->getPlayerColor(player->getId())->name + ".xml";
		perPlayerAura.push_back(createSet(material));
	}
	for (int i = 0; i < Game::getDatabase()->getResourceSize(); ++i) {
		auto material = "Materials/select/select_grey_" + Game::getDatabase()->getResource(i)->name + ".xml";
		resourceAura.push_back(createSet(material));
		material = "Materials/bar/bar_grey.xml";
		resourceBar.push_back(createSet(material));
	}
}

BillboardSetProvider::~BillboardSetProvider() {
	node->Remove();
}

void BillboardSetProvider::reset() {
	auraIdx = 0;
	barIdx = 0;
}

Urho3D::Billboard* BillboardSetProvider::getNextBar(ObjectType type, char player, char id) {
	switch (type) {
	case ObjectType::UNIT:
	case ObjectType::BUILDING:
		return perPlayerAura[player]->GetBillboard(barIdx++);
	case ObjectType::RESOURCE:
		return resourceAura[id]->GetBillboard(barIdx++);
	default: return nullptr;
	}
}

Urho3D::Billboard* BillboardSetProvider::getNextAura(ObjectType type, char player, char id) {
	switch (type) {
	case ObjectType::UNIT:
	case ObjectType::BUILDING:
		return perPlayerBar[player]->GetBillboard(auraIdx++);
	case ObjectType::RESOURCE:
		return resourceBar[id]->GetBillboard(auraIdx++);
	default: return nullptr;
	}
}
