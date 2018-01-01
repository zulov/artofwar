#include "objects/Physical.h"
#include "Game.h"

#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include "objects/LinkComponent.h"
#include <string>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Resource/ResourceCache.h>

Physical::Physical(Vector3* _position, ObjectType _type): Entity(_type) {
	LinkComponent* lc = node->CreateComponent<LinkComponent>();
	lc->bound(this);

	position = _position;
	rotation = new Vector3();

	node->SetPosition(*position);

	for (int & bucket : bucketIndex) {
		bucket = INT_MIN;
	}
	bucketIndexShift = bucketIndex + 1;
}

Physical::~Physical() {
	delete position;
	delete rotation;
}


void Physical::initBillbords() {
	StaticModel* model = node->GetComponent<StaticModel>();
	if (model) {
		billboardNode = node->CreateChild();
		billboardNode->Pitch(90);
		barNode = node->CreateChild();
		Model* model3d = model->GetModel();//TODO razy scale?
		const Vector3 boundingBox = model3d->GetBoundingBox().Size();

		billboardSetBar = barNode->CreateComponent<BillboardSet>();
		billboardSetBar->SetNumBillboards(1);
		billboardSetBar->SetMaterial(Game::get()->getCache()->GetResource<Material>("Materials/red.xml"));
		billboardSetBar->SetSorted(true);

		billboardBar = billboardSetBar->GetBillboard(0);
		billboardBar->size_ = Vector2(2, 0.1);
		billboardBar->position_ = Vector3(0, boundingBox.y_ * 1.3f, 0);
		billboardBar->enabled_ = false;

		billboardSetShadow = billboardNode->CreateComponent<BillboardSet>();
		billboardSetShadow->SetNumBillboards(1);
		billboardSetShadow->SetMaterial(Game::get()->getCache()->GetResource<Material>("Materials/select.xml"));
		billboardSetShadow->SetSorted(true);
		billboardSetShadow->SetFaceCameraMode(FaceCameraMode::FC_NONE);

		billboardShadow = billboardSetShadow->GetBillboard(0);
		billboardShadow->size_ = Vector2(boundingBox.x_ * 1.3f, boundingBox.z_ * 1.3f);
		billboardShadow->position_ = Vector3(0, 0, -0.3);
		billboardShadow->enabled_ = false;

		billboardSetBar->Commit();
		billboardSetShadow->Commit();
	}
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

void Physical::action(short id, ActionParameter* parameter) {

}

std::string Physical::getColumns() {
	return Entity::getColumns() 
	+ "hp_coef		INT     NOT NULL,"
	+ "player		INT     NOT NULL,";
}

std::string Physical::getValues(int precision) {
	int hp_coef = getHealthPercent() * precision;
	return Entity::getValues(precision) 
	+ std::to_string(hp_coef) + ","
	+ std::to_string(player) + ",";
}

bool Physical::hasEnemy() {
	if (enemyToAttack != nullptr) {
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
	if (type == PHISICAL) { return; }
	billboardBar->enabled_ = true;
	billboardShadow->enabled_ = true;
	updateHealthBar();
}

void Physical::unSelect() {
	if (type == PHISICAL) { return; }
	billboardBar->enabled_ = false;
	billboardShadow->enabled_ = false;

	billboardSetBar->Commit();
	billboardSetShadow->Commit();
}
