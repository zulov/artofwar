#include "BillboardSetProvider.h"


#include <Urho3D/Graphics/BillboardSet.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/ResourceCache.h>
#include "Game.h"
#include "database/DatabaseCache.h"
#include "objects/ObjectEnums.h"
#include "player/Player.h"
#include "player/PlayersManager.h"

BillboardSetProvider::BillboardSetProvider() {
	node = Game::getScene()->CreateChild();
	nodeAura = Game::getScene()->CreateChild();
}

BillboardSetProvider::~BillboardSetProvider() {
	node->Remove();
	nodeAura->Remove();
}

void BillboardSetProvider::init() {
	for (auto player : Game::getPlayersMan()->getAllPlayers()) {
		auto material = "Materials/select/select_" + Game::getDatabase()->getPlayerColor(player->getId())->name +
			".xml";
		auto bs = createSet(nodeAura, material);
		bs->SetFaceCameraMode(Urho3D::FaceCameraMode::FC_NONE);
		bs->SetRelative(false);
		perPlayerAura.push_back(bs);
		material = "Materials/bar/bar_" + Game::getDatabase()->getPlayerColor(player->getId())->name + ".xml";
		perPlayerBar.push_back(createSet(node, material));
	}
	nodeAura->Pitch(90);
	for (int i = 0; i < Game::getDatabase()->getResourceSize(); ++i) {
		auto material = "Materials/select/select_grey_" + Game::getDatabase()->getResource(i)->name + ".xml";
		auto bs = createSet(nodeAura, material);
		bs->SetFaceCameraMode(Urho3D::FaceCameraMode::FC_NONE);
		bs->SetRelative(false);
		resourceAura.push_back(bs);
		material = "Materials/bar/bar_grey.xml";
		resourceBar.push_back(createSet(node, material));
	}
}

Urho3D::BillboardSet* BillboardSetProvider::createSet(Urho3D::Node* node, Urho3D::String& materialName) const {
	auto bs = node->CreateComponent<Urho3D::BillboardSet>();
	bs->SetNumBillboards(10000);

	bs->SetMaterial(Game::getCache()->GetResource<Urho3D::Material>(materialName));
	bs->SetSorted(true);
	return bs;
}

void BillboardSetProvider::reset() {
	auraIdx = 0;
	barIdx = 0;
	commit();
}

Urho3D::Billboard* BillboardSetProvider::getNextAura(ObjectType type, char player, short id) {
	switch (type) {
	case ObjectType::UNIT:
	case ObjectType::BUILDING:
		return perPlayerAura[player]->GetBillboard(barIdx++);
	case ObjectType::RESOURCE:
		return resourceAura[id]->GetBillboard(barIdx++);
	default: return nullptr;
	}
}

Urho3D::Billboard* BillboardSetProvider::getNextBar(ObjectType type, char player, short id) {
	switch (type) {
	case ObjectType::UNIT:
	case ObjectType::BUILDING:
		return perPlayerBar[player]->GetBillboard(auraIdx++);
	case ObjectType::RESOURCE:
		return resourceBar[id]->GetBillboard(auraIdx++);
	default: return nullptr;
	}
}

void BillboardSetProvider::commit() {
	for (auto billboardSet : perPlayerAura) {
		billboardSet->Commit();
	}
	for (auto billboardSet : resourceAura) {
		billboardSet->Commit();
	}
	for (auto billboardSet : perPlayerBar) {
		billboardSet->Commit();
	}
	for (auto billboardSet : resourceBar) {
		billboardSet->Commit();
	}
}
