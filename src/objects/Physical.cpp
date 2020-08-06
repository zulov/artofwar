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
}


bool Physical::isAlive() const {
	return true;
}

short Physical::getId() {
	return -1;
}

float Physical::getShadowSize(const Urho3D::Vector3& boundingBox) const {
	return (boundingBox.x_ + boundingBox.z_) / 2 * 1.2f;
}

void Physical::updateBillboardShadow(Urho3D::Vector3& boundingBox) const {
	const auto boudingSize = getShadowSize(boundingBox);
	if (selected && billboardShadow1) {
		billboardShadow1->position_ = position;
		billboardShadow1->size_ = {boudingSize, boudingSize};
		billboardShadow1->enabled_ = true;
	}
}

void Physical::updateBillboardBar(Urho3D::Vector3& boundingBox) const {
	if (selected && billboardBar1) {
		billboardBar1->position_ = position + Urho3D::Vector3{0, boundingBox.y_ * 1.3f, 0};
		billboardBar1->size_ = Urho3D::Vector2(getHealthBarSize(), getHealthBarThick());
		billboardBar1->enabled_ = true;
	}
}

void Physical::updateBillboards() const {
	auto boundingBox = model->GetModel()->GetBoundingBox().Size();
	if (selected) {
		updateBillboardBar(boundingBox);
		updateBillboardShadow(boundingBox);
	}
}

void Physical::updateHealthBar() const {
	if (selected && billboardBar1) {
		billboardBar1->size_ = Urho3D::Vector2(getHealthBarSize(), getHealthBarThick());
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

void Physical::select(Urho3D::Billboard* billboardBar1, Urho3D::Billboard* billboardShadow1) {
	if (getType() == ObjectType::PHYSICAL) { return; }
	selected = true;
	// billboardBar->enabled_ = true;
	// billboardShadow->enabled_ = true;
	this->billboardBar1 = billboardBar1;
	this->billboardShadow1 = billboardShadow1;
	updateBillboards();
	// if (this->billboardBar1) {
	// 	this->billboardBar1->position_ = position + Urho3D::Vector3{0, 10 * 1.3f, 0};
	// 	this->billboardBar1->size_ = {2, 0.1};
	// 	this->billboardBar1->enabled_ = true;
	// 	//billboardBar1->screenScaleFactor_
	// }

	// if (this->billboardShadow1) {
	// 	this->billboardShadow1->position_ = position;
	// 	this->billboardShadow1->size_ = {10, 10};
	// 	this->billboardShadow1->enabled_ = true;
	// }

	updateHealthBar();
}

void Physical::unSelect() {
	if (getType() == ObjectType::PHYSICAL) { return; }
	selected = false;
	// billboardBar->enabled_ = false;
	// billboardShadow->enabled_ = false;
	//
	// billboardSetBar->Commit();
	// billboardSetShadow->Commit();

	if (billboardBar1) {
		billboardBar1->enabled_ = false;
		billboardBar1 = nullptr;
	}

	if (billboardShadow1) {
		billboardShadow1->enabled_ = false;
		billboardShadow1 = nullptr;
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
