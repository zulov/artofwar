#include "objects/Physical.h"
#include <Urho3D/Graphics/BillboardSet.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Scene/Scene.h>
#include "Game.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "scene/load/dbload_container.h"


Physical::Physical(Urho3D::Vector3& _position):
	position(_position), indexInGrid(INT_MIN) {
	node = Game::getScene()->CreateChild();
	node->SetVar("link", this);
	node->SetPosition(position);
	isVisible = true;
}

Physical::~Physical() {
	node->Remove();
	unSelect();
}


bool Physical::isAlive() const {
	return true;
}

short Physical::getId() {
	return -1;
}

float Physical::getAuraSize(const Urho3D::Vector3& boundingBox) const {
	return (boundingBox.x_ + boundingBox.z_) / 2 * 1.2f;
}

void Physical::updateBillboardAura(Urho3D::Vector3& boundingBox) const {
	if (selected && aura) {
		const auto auraSize = getAuraSize(boundingBox);
		aura->position_ = node->GetPosition();
		aura->size_ = {auraSize, auraSize};
		aura->enabled_ = true;
	}
}

void Physical::updateBillboardBar(Urho3D::Vector3& boundingBox) const {
	if (selected && healthBar) {
		healthBar->position_ = node->GetPosition() + Urho3D::Vector3{0, boundingBox.y_ * 1.3f, 0};
		healthBar->size_ = {getHealthBarSize(), getHealthBarThick()};
		healthBar->enabled_ = true;
	}
}

void Physical::updateBillboards() const {
	if (selected) {
		auto b = model->GetModel()->GetBoundingBox().Size() * node->GetScale();
		updateBillboardBar(b);
		updateBillboardAura(b);
	}
}

void Physical::updateHealthBar() const {
	if (selected && healthBar) {
		healthBar->size_ = Urho3D::Vector2(getHealthBarSize(), getHealthBarThick());
	}
}

int Physical::getMainCell() const {
	return getMainBucketIndex();
}

std::optional<std::tuple<Urho3D::Vector2, float, int>> Physical::getPosToUseWithIndex(Unit* user) {
	return {};
}

std::optional<Urho3D::Vector2> Physical::getPosToUseBy(Unit* follower) {
	auto a = getPosToUseWithIndex(follower);
	if (a.has_value()) {
		return std::get<0>(a.value());
	}
	return {};
}

float Physical::getHealthBarSize() const {
	auto healthBarSize = getMaxHpBarSize() * getHealthPercent();
	if (healthBarSize <= 0) { healthBarSize = 0; }
	return healthBarSize;
}

bool Physical::isSelected() const {
	assert(getType() != ObjectType::PHYSICAL);
	return selected;
}

void Physical::load(dbload_physical* dbloadPhysical) {
	hp = dbloadPhysical->hp;
}

Urho3D::String Physical::toMultiLineString() {
	return "Physical";
}

char Physical::getLevelNum() {
	return -1;
}

std::string Physical::getValues(int precision) {
	int hp_coef = hp * precision;
	return std::to_string(getId()) + ","
		+ std::to_string(hp_coef) + ","
		+ std::to_string(player) + ","
		+ std::to_string(getLevelNum()) + ",";
}

int Physical::belowCloseLimit() {
	return Urho3D::Max(getMaxCloseUsers() - closeUsers, 0);
}

void Physical::select(Urho3D::Billboard* healthBar, Urho3D::Billboard* aura) {
	if (getType() == ObjectType::PHYSICAL) { return; }
	assert(selected==false);
	selected = true;
	this->healthBar = healthBar;
	this->aura = aura;
	updateBillboards();
	updateHealthBar();
}

void Physical::disableBillboard(Urho3D::Billboard* billboard) {
	if (billboard) {
		billboard->enabled_ = false;
		billboard = nullptr;
	}
}

void Physical::unSelect() {
	if (getType() == ObjectType::PHYSICAL) { return; }
	selected = false;

	disableBillboard(healthBar);
	disableBillboard(aura);
}

void Physical::loadXml(const Urho3D::String& xmlName) {
	//node->RemoveAllChildren();
	node->LoadXML(Game::getCache()->GetResource<Urho3D::XMLFile>(xmlName)->GetRoot());

	node->SetVar("link", this);

	model = node->GetComponent<Urho3D::StaticModel>();

	populate();
	updateBillboards();
}


void Physical::setPlayerAndTeam(int player) {
	setPlayer(player);
	setTeam(Game::getPlayersMan()->getPlayer(player)->getTeam());
}
