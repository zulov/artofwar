#include "objects/Physical.h"
#include "Game.h"
#include "MathUtils.h"
#include "ObjectEnums.h"
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <algorithm>
#include <string>


Physical::Physical(Urho3D::Vector3* _position, ObjectType _type): Entity(_type), position(_position) {
	node->SetVar("link", this);
	node->SetPosition(*position);

	indexInGrid = INT_MIN;
}

Physical::~Physical() {
	delete position;
}


void Physical::createBillboardBar() {
	billboardBar = createBillboardSet(barNode, billboardSetBar, "Materials/red_overlay.xml");
}

void Physical::createBillboardShadow() {
	billboardShadow = createBillboardSet(billboardNode, billboardSetShadow, "Materials/select.xml");
	billboardNode->Pitch(90);
	billboardSetShadow->SetFaceCameraMode(Urho3D::FaceCameraMode::FC_NONE);
}

Urho3D::Billboard* Physical::createBillboardSet(Urho3D::Node*& node, Urho3D::BillboardSet*& billbordSet,
                                                const Urho3D::String& material) const {
	node = this->node->CreateChild();
	auto test = node->GetScale();

	billbordSet = node->CreateComponent<Urho3D::BillboardSet>();
	billbordSet->SetNumBillboards(1);
	billbordSet->SetMaterial(Game::getCache()->GetResource<Urho3D::Material>(material));
	billbordSet->SetSorted(true);
	return billbordSet->GetBillboard(0);
}

void Physical::updateBillboardShadow(Urho3D::Vector3& boundingBox) const {
	const auto boudingSize = (boundingBox.x_ + boundingBox.z_) / 2 * 1.2f;
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
	auto boundingBox = node->GetComponent<Urho3D::StaticModel>()->GetModel()->GetBoundingBox().Size();

	updateBillboardBar(boundingBox);
	updateBillboardShadow(boundingBox);
}

void Physical::initBillboards() {
	createBillboardBar();
	createBillboardShadow();
}

void Physical::updateHealthBar() {
	if (billboardBar->enabled_) {
		billboardBar->size_ = Urho3D::Vector2(getHealthBarSize(), 0.1) / node->GetScale2D();
		billboardSetBar->Commit();
	}
}

int Physical::getMainCell() const {
	return getBucketIndex();
}

std::tuple<Urho3D::Vector2, int> Physical::getPosToFollowWithIndex(Urho3D::Vector3* center) const {
	return {getPosToFollow(center), -1};
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
}

void Physical::setTeam(unsigned char _team) {
	team = _team;
}

void Physical::setPlayer(unsigned char player) {
	this->player = player;
}

bool Physical::isSelected() const {
	return type != ObjectType::PHYSICAL && billboardBar->enabled_;
}

void Physical::load(dbload_physical* dbloadPhysical) {
	hpCoef = maxHpCoef * dbloadPhysical->hp_coef;
}

Urho3D::String& Physical::toMultiLineString() {
	return menuString;
}

std::string Physical::getColumns() {
	return Entity::getColumns()
		+ "hp_coef		INT     NOT NULL,"
		+ "player		INT     NOT NULL,"
		+ "level		INT     NOT NULL,";
}

int Physical::getLevel() {
	return -1;
}

std::string Physical::getValues(int precision) {
	int hp_coef = getHealthPercent() * precision;
	return Entity::getValues(precision)
		+ std::to_string(hp_coef) + ","
		+ std::to_string(player) + ","
		+ std::to_string(getLevel()) + ",";
}

bool Physical::isFirstThingAlive() {
	return !thingsToInteract.empty()
		&& thingsToInteract[0] != nullptr
		&& thingsToInteract[0]->isUsable();
}

int Physical::belowCloseLimit() {
	auto diff = maxCloseUsers - closeUsers;
	return diff > 0 ? diff : 0;
}

bool Physical::hasEnemy() {
	if (isFirstThingAlive()) {
		if (sqDist(this->getPosition(), thingsToInteract[0]->getPosition()) < attackRange * attackRange) {
			return true;
		}
	}
	thingsToInteract.clear();
	return false;
}

void Physical::clean() {
	thingsToInteract.erase(
	                       std::remove_if(
	                                      thingsToInteract.begin(), thingsToInteract.end(),
	                                      [](Physical* physical)
	                                      {
		                                      return physical == nullptr || !physical->isAlive();
	                                      }),
	                       thingsToInteract.end());
}

void Physical::select() {
	if (type == ObjectType::PHYSICAL) { return; }
	billboardBar->enabled_ = true;
	billboardShadow->enabled_ = true;
	updateHealthBar();
}

void Physical::unSelect() {
	if (type == ObjectType::PHYSICAL) { return; }
	billboardBar->enabled_ = false;
	billboardShadow->enabled_ = false;

	billboardSetBar->Commit();
	billboardSetShadow->Commit();
}


std::tuple<Urho3D::Vector2, float, int> Physical::closest(Physical* physical, Urho3D::Vector3* mainPos,
                                                          const std::function<
	                                                          std::tuple<Urho3D::Vector2, int>(
		                                                          Physical*,
		                                                          Urho3D::Vector3*)>&
                                                          positionFunc) {
	auto [pos, indexOfPos] = positionFunc(physical, mainPos);
	const float distance = sqDist(pos, *mainPos);
	return std::tuple<Urho3D::Vector2, float, int>(pos, distance, indexOfPos);
}

std::tuple<Physical*, float, int> Physical::closestPhysical(std::vector<Physical*>* things,
                                                            const std::function<bool(Physical*)>& condition,
                                                            const std::function<
	                                                            std::tuple<Urho3D::Vector2, int>(
		                                                            Physical*,
		                                                            Urho3D::Vector3*)>&
                                                            positionFunc) {
	float minDistance = 99999;
	Physical* closestPhy = nullptr;
	int bestIndex = -1;
	for (auto entity : *things) {
		if (entity->isAlive() && condition(entity)) {
			auto [pos, distance, indexOfPos] = closest(entity, position, positionFunc);

			if (distance <= minDistance) {
				minDistance = distance;
				closestPhy = entity;
				bestIndex = indexOfPos;
			}
		}
	}
	return std::tuple<Physical*, float, int>(closestPhy, minDistance, bestIndex);
}
