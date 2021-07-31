#include "BillboardSetProvider.h"
#include <Urho3D/Graphics/BillboardSet.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/ResourceCache.h>
#include "Game.h"
#include "database/DatabaseCache.h"
#include "database/db_other_struct.h"
#include "objects/ObjectEnums.h"
#include "objects/SelectedObject.h"
#include "player/Player.h"
#include "player/PlayersManager.h"


BillboardSetProvider::BillboardSetProvider() {
	nodeBar = Game::getScene()->CreateChild();
	nodeAura = Game::getScene()->CreateChild();
}

BillboardSetProvider::~BillboardSetProvider() {
	nodeBar->Remove();
	nodeAura->Remove();
	delete[]objects;
}

void BillboardSetProvider::init() {
	for (int i = 0; i < Game::getPlayersMan()->getAllPlayers().size(); ++i) {
		const auto player = Game::getPlayersMan()->getAllPlayers()[i];
		auto material = "Materials/select/select_" + Game::getDatabase()->getPlayerColor(player->getId())->name +
			".xml";
		auto bs = createSet(nodeAura, material, PLAYER_SIZE);
		bs->SetFaceCameraMode(Urho3D::FaceCameraMode::FC_NONE);
		bs->SetRelative(false);
		perPlayerAura[i] = bs;
		material = "Materials/bar/bar_" + Game::getDatabase()->getPlayerColor(player->getId())->name + ".xml";
		perPlayerBar[i] = createSet(nodeBar, material, PLAYER_SIZE);
	}
	nodeAura->Pitch(90);
	for (int i = 0; i < RESOURCES_SIZE; ++i) {
		auto material = "Materials/select/select_grey_" + Game::getDatabase()->getResource(i)->name + ".xml";
		auto bs = createSet(nodeAura, material, RESOURCE_SIZE);
		bs->SetFaceCameraMode(Urho3D::FaceCameraMode::FC_NONE);
		bs->SetRelative(false);
		resourceAura[i] = bs;
	}
	Urho3D::String material = "Materials/bar/bar_grey.xml";
	resourceBar = createSet(nodeBar, material, RESOURCE_SIZE);
	objects = new SelectedObject[VECTOR_SIZE];
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
	idx = 0;
	commit();
}

SelectedObject* BillboardSetProvider::getNext(ObjectType type, char player, short id) {
	Urho3D::Billboard* aura{};
	Urho3D::Billboard* bar{};
	switch (type) {
	case ObjectType::UNIT:
	case ObjectType::BUILDING:
		aura = perPlayerAura[player]->GetBillboard(idx);
		bar = perPlayerBar[player]->GetBillboard(idx);
		break;
	case ObjectType::RESOURCE:
		aura = resourceAura[id]->GetBillboard(idx);
		bar = resourceBar->GetBillboard(idx);
		break;
	}
	++idx;
	if (!aura && !bar) {
		return &EMPTY;
	}
	objects[idx].set(aura, bar);
	return &objects[idx];
}


void BillboardSetProvider::commit() {
	auto func = [](Urho3D::BillboardSet* b){ b->Commit(); };
	std::ranges::for_each(perPlayerAura, func);
	std::ranges::for_each(resourceAura, func);
	std::ranges::for_each(perPlayerBar, func);
	resourceBar->Commit();
}
