#include "objects/Physical.h"
#include "Game.h"
#include "objects/ObjectEnums.h"
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <string>
#include <Urho3D/Resource/XMLFile.h>
#include "player/PlayersManager.h"
#include "player/Player.h"
#include "unit/Unit.h"
#include "database/DatabaseCache.h"


Physical::Physical(Urho3D::Vector3& _position):
	position(_position), indexInGrid(INT_MIN) {
	node = Game::getScene()->CreateChild();
	node->SetVar("link", this);
	node->SetPosition(position);
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
	if (selected && billboardAura) {
		const auto boudingSize = getAuraSize(boundingBox);
		billboardAura->position_ = position;
		billboardAura->size_ = {boudingSize, boudingSize};
		billboardAura->enabled_ = true;
	}
}

void Physical::updateBillboardBar(Urho3D::Vector3& boundingBox) const {
	if (selected && billboardBar) {
		billboardBar->position_ = position + Urho3D::Vector3{0, boundingBox.y_ * 1.3f, 0};
		billboardBar->size_ = {getHealthBarSize(), getHealthBarThick()};
		billboardBar->enabled_ = true;
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
	if (selected && billboardBar) {
		billboardBar->size_ = Urho3D::Vector2(getHealthBarSize(), getHealthBarThick());
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

bool Physical::bucketHasChanged(int _bucketIndex) const {
	return indexInGrid != _bucketIndex;
}

void Physical::setBucket(int _bucketIndex) {
	indexInGrid = _bucketIndex;
	indexHasChanged = true;
}

void Physical::setTeam(unsigned char _team) {
	team = _team;
}

void Physical::setPlayer(unsigned char player) {
	this->player = player;
}

bool Physical::isSelected() const {
	assert(getType() != ObjectType::PHYSICAL);
	return selected;
}

void Physical::load(dbload_physical* dbloadPhysical) {
	hp = maxHp * dbloadPhysical->hp_coef;
}

Urho3D::String Physical::toMultiLineString() {
	return "Physical";
}

void Physical::indexHasChangedReset() {
	indexHasChanged = false;
}

std::string Physical::getColumns() {
	return "id_db	INT		NOT NULL,"
		"hp_coef	INT     NOT NULL,"
		"player		INT     NOT NULL,"
		"level		INT     NOT NULL,";
}

int Physical::getLevel() {
	return -1;
}

std::string Physical::getValues(int precision) {
	int hp_coef = getHealthPercent() * precision;
	return std::to_string(getId()) + ","
		+ std::to_string(hp_coef) + ","
		+ std::to_string(player) + ","
		+ std::to_string(getLevel()) + ",";
}

int Physical::belowCloseLimit() {
	return Urho3D::Max(maxCloseUsers - closeUsers, 0);
}

ObjectType Physical::getType() const {
	return ObjectType::PHYSICAL;
}

void Physical::select(Urho3D::Billboard* billboardBar, Urho3D::Billboard* billboardAura) {
	if (getType() == ObjectType::PHYSICAL) { return; }
	assert(selected==false);
	selected = true;
	this->billboardBar = billboardBar;
	this->billboardAura = billboardAura;
	updateBillboards();
	updateHealthBar();
}

void Physical::unSelect() {
	if (getType() == ObjectType::PHYSICAL) { return; }
	selected = false;

	if (billboardBar) {
		billboardBar->enabled_ = false;
		billboardBar = nullptr;
	}

	if (billboardAura) {
		billboardAura->enabled_ = false;
		billboardAura = nullptr;
	}
}

float Physical::getValueOf(ValueType type) const {
	return -1;
}

void Physical::fillValues(std::span<float> weights) const {
}

void Physical::addValues(std::span<float> vals) const {
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
