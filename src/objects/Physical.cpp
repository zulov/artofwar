#include "objects/Physical.h"
#include "Game.h"
#include "objects/LinkComponent.h"
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <string>


Physical::Physical(Vector3* _position, ObjectType _type): Entity(_type) {
	LinkComponent* lc = node->CreateComponent<LinkComponent>();
	lc->bound(this);

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


void Physical::createBillboardBar(Vector3& boundingBox) {
	barNode = node->CreateChild();

	billboardSetBar = barNode->CreateComponent<BillboardSet>();
	billboardSetBar->SetNumBillboards(1);
	billboardSetBar->SetMaterial(Game::get()->getCache()->GetResource<Material>("Materials/red.xml"));
	billboardSetBar->SetSorted(true);

	billboardBar = billboardSetBar->GetBillboard(0);
	billboardBar->size_ = Vector2(2, 0.1);
	billboardBar->position_ = Vector3(0, boundingBox.y_ * 1.3f, 0);
	billboardBar->enabled_ = false;

	billboardSetBar->Commit();
}

void Physical::createBillboardShadow(Vector3& boundingBox) {
	billboardNode = node->CreateChild();
	billboardNode->Pitch(90);
	billboardSetShadow = billboardNode->CreateComponent<BillboardSet>();
	billboardSetShadow->SetNumBillboards(1);
	billboardSetShadow->SetMaterial(Game::get()->getCache()->GetResource<Material>("Materials/select.xml"));
	billboardSetShadow->SetSorted(true);
	billboardSetShadow->SetFaceCameraMode(FaceCameraMode::FC_NONE);


	billboardShadow = billboardSetShadow->GetBillboard(0);
	billboardShadow->position_ = Vector3(0, 0, -0.3 / node->GetScale().x_);
	float boudingSize = (boundingBox.x_ + boundingBox.z_) / 2 * 1.3f ;
	billboardShadow->size_ = Vector2(boudingSize, boudingSize);
	billboardShadow->enabled_ = false;

	billboardSetShadow->Commit();
}

void Physical::initBillbords() {
	StaticModel* model = node->GetComponent<StaticModel>();

	Vector3 boundingBox = model->GetModel()->GetBoundingBox().Size(); //TODO razy scale?

	createBillboardBar(boundingBox);

	createBillboardShadow(boundingBox);

}

void Physical::updateHealthBar() {
	float healthBarSize = getHealthBarSize();

	billboardBar->size_ = Vector2(healthBarSize, 0.1) / node->GetScale2D();
	billboardSetBar->Commit();
}

float Physical::getHealthBarSize() {
	return 1;
}

float Physical::getHealthPercent() {
	return 1.0;
}

int Physical::getBucketIndex(char param) {
	return bucketIndexShift[param];
}

bool Physical::bucketHasChanged(int _bucketIndex, char param) {
	if (bucketIndexShift[param] == _bucketIndex) {
		return false;
	}
	return true;
}

void Physical::setBucket(int _bucketIndex, char param) {
	bucketIndexShift[param] = _bucketIndex;
}

void Physical::setTeam(unsigned char _team) {
	team = _team;
}

void Physical::setPlayer(unsigned char player) {
	this->player = player;
}

unsigned char Physical::getPlayer() {
	return player;
}

String& Physical::toMultiLineString() {
	return menuString;
}

void Physical::action(short id, ActionParameter& parameter) {

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

bool Physical::hasEnemy() {
	if (enemyToAttack != nullptr && enemyToAttack->isAlive()) {
		if ((*this->getPosition() - *enemyToAttack->getPosition()).LengthSquared() < attackRange * attackRange) {
			return true;
		}
	}
	enemyToAttack = nullptr;
	return false;
}

void Physical::clean() {
	if (enemyToAttack != nullptr && !enemyToAttack->isAlive()) {
		enemyToAttack = nullptr;
	}
}

signed char Physical::getTeam() {
	return team;
}

void Physical::absorbAttack(double attackCoef) {

}

Urho3D::Vector3* Physical::getPosition() {
	return position;
}


void Physical::select() {
	if (type == ObjectType::PHISICAL) { return; }
	billboardBar->enabled_ = true;
	billboardShadow->enabled_ = true;
	updateHealthBar();
}

void Physical::unSelect() {
	if (type == ObjectType::PHISICAL) { return; }
	billboardBar->enabled_ = false;
	billboardShadow->enabled_ = false;

	billboardSetBar->Commit();
	billboardSetShadow->Commit();
}
