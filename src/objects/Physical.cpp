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

Urho3D::String Physical::getBarMaterialName() {
	if (player == -1) {
		return "Materials/bar/bar_grey.xml";
	}
	return "Materials/bar/bar_" + Game::getDatabase()->getPlayerColor(player)->name + ".xml";
}

void Physical::createBillboardBar() {
	billboardBar = createBillboardSet(barNode, billboardSetBar, getBarMaterialName());
}

void Physical::createBillboardShadow() {
	Urho3D::String material;

	if (player == -1) {
		material = "Materials/select/select_grey.xml";
	} else {
		material = "Materials/select/select_" + Game::getDatabase()->getPlayerColor(player)->name + ".xml";
	}
	billboardShadow = createBillboardSet(billboardNode, billboardSetShadow, material);
	billboardNode->Pitch(90);
	billboardSetShadow->SetFaceCameraMode(Urho3D::FaceCameraMode::FC_NONE);
}

float Physical::getShadowSize(const Urho3D::Vector3& boundingBox) const {
	return (boundingBox.x_ + boundingBox.z_) / 2 * 1.2f;
}

Urho3D::Billboard* Physical::createBillboardSet(Urho3D::Node*& node, Urho3D::BillboardSet*& billbordSet,
                                                const Urho3D::String& material) const {
	node = this->node->CreateChild();

	billbordSet = node->CreateComponent<Urho3D::BillboardSet>();
	billbordSet->SetNumBillboards(1);
	billbordSet->SetMaterial(Game::getCache()->GetResource<Urho3D::Material>(material));
	billbordSet->SetSorted(true);
	return billbordSet->GetBillboard(0);
}

void Physical::updateBillboardShadow(Urho3D::Vector3& boundingBox) const {
	const auto boudingSize = getShadowSize(boundingBox);
	billboardShadow->size_ = {boudingSize, boudingSize};
	billboardShadow->enabled_ = false;

	billboardSetShadow->Commit();
}

void Physical::updateBillboardBar(Urho3D::Vector3& boundingBox) const {
	billboardBar->position_ = {0, boundingBox.y_ * 1.3f, 0};
	billboardBar->size_ = {2, 0.1};
	billboardBar->enabled_ = false;

	billboardSetBar->Commit();
}

void Physical::updateBillboards() const {
	auto boundingBox = model->GetModel()->GetBoundingBox().Size();

	updateBillboardBar(boundingBox);
	updateBillboardShadow(boundingBox);
}

void Physical::initBillboards() {
	createBillboardBar();
	createBillboardShadow();
}

void Physical::updateHealthBar() {
	if (billboardBar->enabled_) {
		billboardBar->size_ = Urho3D::Vector2(getHealthBarSize(), getHealthBarThick()) / node->GetScale2D();
		billboardSetBar->Commit();
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

float Physical::getHealthBarSize() {
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
	return getType() != ObjectType::PHYSICAL && billboardBar->enabled_;
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

void Physical::select() {
	if (getType() == ObjectType::PHYSICAL) { return; }
	billboardBar->enabled_ = true;
	billboardShadow->enabled_ = true;
	updateHealthBar();
}

void Physical::unSelect() {
	if (getType() == ObjectType::PHYSICAL) { return; }
	billboardBar->enabled_ = false;
	billboardShadow->enabled_ = false;

	billboardSetBar->Commit();
	billboardSetShadow->Commit();
}

float Physical::getValueOf(ValueType type) const {
	return -1;
}

void Physical::loadXml(const Urho3D::String& xmlName) {
	//node->RemoveAllChildren();
	node->LoadXML(Game::getCache()->GetResource<Urho3D::XMLFile>(xmlName)->GetRoot());

	node->SetVar("link", this);
	initBillboards();

	model = node->GetComponent<Urho3D::StaticModel>();
	populate();
	updateBillboards();
}


void Physical::setPlayerAndTeam(int player) {
	setPlayer(player);
	setTeam(Game::getPlayersMan()->getPlayer(player)->getTeam());
}
