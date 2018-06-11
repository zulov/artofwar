#include "objects/Physical.h"
#include "Game.h"
#include "objects/LinkComponent.h"
#include "MathUtils.h"
#include "ObjectEnums.h"
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <algorithm>
#include <string>


Physical::Physical(Vector3* _position, ObjectType _type): Entity(_type) {
	node->CreateComponent<LinkComponent>()->bound(this);

	position = _position;
	node->SetPosition(*position);

	for (int& bucket : bucketIndex) {
		bucket = INT_MIN;
	}
	bucketIndexShift = bucketIndex + 1;
}

Physical::~Physical() {
	delete position;
}


void Physical::createBillboardBar() {
	barNode = node->CreateChild();

	billboardSetBar = barNode->CreateComponent<BillboardSet>();
	billboardSetBar->SetNumBillboards(1);
	billboardSetBar->SetMaterial(Game::getCache()->GetResource<Material>("Materials/red_overlay.xml"));
	billboardSetBar->SetSorted(true);

	billboardBar = billboardSetBar->GetBillboard(0);
}

void Physical::updateBillboardBar(Vector3& boundingBox) const {
	billboardBar->size_ = Vector2(2, 0.1);
	billboardBar->position_ = Vector3(0, boundingBox.y_ * 1.3f, 0);
	billboardBar->enabled_ = false;

	billboardSetBar->Commit();
}

void Physical::createBillboardShadow() {
	billboardNode = node->CreateChild();
	billboardNode->Pitch(90);
	billboardSetShadow = billboardNode->CreateComponent<BillboardSet>();
	billboardSetShadow->SetNumBillboards(1);
	billboardSetShadow->SetMaterial(Game::getCache()->GetResource<Material>("Materials/select.xml"));
	billboardSetShadow->SetSorted(true);
	billboardSetShadow->SetFaceCameraMode(FaceCameraMode::FC_NONE);

	billboardShadow = billboardSetShadow->GetBillboard(0);
}

void Physical::updateBillboardShadow(Vector3& boundingBox) const {
	const float boudingSize = (boundingBox.x_ + boundingBox.z_) / 2 * 1.3f;
	billboardShadow->size_ = Vector2(boudingSize, boudingSize);
	billboardShadow->enabled_ = false;

	billboardSetShadow->Commit();
}

void Physical::updateBillbords() const {
	Vector3 boundingBox = node->GetComponent<StaticModel>()->GetModel()->GetBoundingBox().Size();

	updateBillboardBar(boundingBox);
	updateBillboardShadow(boundingBox);
}

void Physical::initBillbords() {
	createBillboardBar();
	createBillboardShadow();
}

void Physical::updateHealthBar() {
	if (billboardBar->enabled_) {
		const float healthBarSize = getHealthBarSize();

		billboardBar->size_ = Vector2(healthBarSize, 0.1) / node->GetScale2D();
		billboardSetBar->Commit();
	}
}

Vector2 Physical::getPosToFollow(Vector3* center) const {
	return Vector2(position->x_, position->z_);
}

float Physical::getMaxHpBarSize() {
	return 0;
}

float Physical::getHealthBarSize() {
	float healthBarSize = getMaxHpBarSize() * getHealthPercent();
	if (healthBarSize <= 0) { healthBarSize = 0; }
	return healthBarSize;
}

bool Physical::bucketHasChanged(int _bucketIndex, char param) const {
	if (bucketIndexShift[param] == _bucketIndex) {
		return false;
	}
	return true;
}

void Physical::setBucket(int _bucketIndex, char param) const {
	bucketIndexShift[param] = _bucketIndex;
}

void Physical::setTeam(unsigned char _team) {
	team = _team;
}

void Physical::setPlayer(unsigned char player) {
	this->player = player;
}

String& Physical::toMultiLineString() {
	return menuString;
}

void Physical::action(char id, ActionParameter& parameter) {
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
		&& thingsToInteract[0]->isAlive();
}

bool Physical::hasEnemy() {
	if (isFirstThingAlive()) {
		if (sqDist(this, thingsToInteract[0]) < attackRange * attackRange) {
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

void Physical::absorbAttack(float attackCoef) {
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
