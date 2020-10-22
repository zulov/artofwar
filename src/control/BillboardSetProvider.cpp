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
	nodeBar = Game::getScene()->CreateChild();
	nodeAura = Game::getScene()->CreateChild();
}

BillboardSetProvider::~BillboardSetProvider() {
	nodeBar->Remove();
	nodeAura->Remove();
}

void BillboardSetProvider::init() {
	for (int i = 0; i < Game::getPlayersMan()->getAllPlayers().size(); ++i) {
		auto player = Game::getPlayersMan()->getAllPlayers()[i];
		auto material = "Materials/select/select_" + Game::getDatabase()->getPlayerColor(player->getId())->name +
			".xml";
		auto bs = createSet(nodeAura, material, 10000);
		bs->SetFaceCameraMode(Urho3D::FaceCameraMode::FC_NONE);
		bs->SetRelative(false);
		perPlayerAura[i] = bs;
		material = "Materials/bar/bar_" + Game::getDatabase()->getPlayerColor(player->getId())->name + ".xml";
		perPlayerBar[i] = createSet(nodeBar, material, 10000);
	}
	nodeAura->Pitch(90);
	for (int i = 0; i < Game::getDatabase()->getResourceSize(); ++i) {
		auto material = "Materials/select/select_grey_" + Game::getDatabase()->getResource(i)->name + ".xml";
		auto bs = createSet(nodeAura, material, 1000);
		bs->SetFaceCameraMode(Urho3D::FaceCameraMode::FC_NONE);
		bs->SetRelative(false);
		resourceAura[i] = bs;
	}
	Urho3D::String material = "Materials/bar/bar_grey.xml";
	resourceBar = createSet(nodeBar, material, 1000);
}

Urho3D::BillboardSet*
BillboardSetProvider::createSet(Urho3D::Node* node, Urho3D::String& materialName, int size) const {
	auto bs = node->CreateComponent<Urho3D::BillboardSet>();
	bs->SetNumBillboards(size);

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
		return perPlayerAura[player]->GetBillboard(auraIdx++);
	case ObjectType::RESOURCE:
		return resourceAura[id]->GetBillboard(auraIdx++);
	default: return nullptr;
	}
}

Urho3D::Billboard* BillboardSetProvider::getNextBar(ObjectType type, char player) {
	switch (type) {
	case ObjectType::UNIT:
	case ObjectType::BUILDING:
		return perPlayerBar[player]->GetBillboard(barIdx++);
	case ObjectType::RESOURCE:
		return resourceBar->GetBillboard(barIdx++);
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
	resourceBar->Commit();
}
