#include "objects/Physical.h"
#include "Game.h"
#include <Urho3D/Graphics/Octree.h>
#include "commands/ActionCommand.h"


Physical::Physical(Vector3* _position, Urho3D::Node* _node, ObjectType _type): Entity(_node, _type) {
	position = _position;
	rotation = new Vector3();

	for (int i = 0; i < BUCKET_SET_NUMBER; ++i) {
		bucketZ[i] = bucketX[i] = INT_MIN;
	}

	StaticModel* model = node->GetComponent<StaticModel>();
	if (model && type != ENTITY && type != PHISICAL) {
		billboardNode = node->CreateChild();
		billboardNode->Pitch(90);
		barNode = node->CreateChild();
		Model* model3d = model->GetModel();//TODO razy scale?
		Vector3 boundingBox = model3d->GetBoundingBox().Size();

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
		billboardShadow->position_ = Vector3(0, 0, -0.1);
		billboardShadow->enabled_ = false;

		billboardSetBar->Commit();
		billboardSetShadow->Commit();
	}

	menuString = new String("Physical");
}

Physical::~Physical() {
	delete position;
	delete rotation;
	delete menuString;
}


void Physical::updateHealthBar() {
	double healthBarSize = getHealthBarSize();

	billboardBar->size_ = Vector2(healthBarSize, 0.1) / node->GetScale2D();
	billboardSetBar->Commit();
}

double Physical::getHealthBarSize() {
	return 1;
}

short Physical::getBucketX(char param) {
	return bucketX[param];
}

short Physical::getBucketZ(char param) {
	return bucketZ[param];
}

bool Physical::bucketHasChanged(short int posX, short int posZ, char param) {
	if (bucketX[param] == posX && bucketZ[param] == posZ) {
		return false;
	}
	return true;
}

void Physical::setBucket(short int posX, short int posZ, char param) {
	bucketX[param] = posX;
	bucketZ[param] = posZ;
}

void Physical::setTeam(char _team) {
	team = _team;
}

void Physical::setPlayer(char player) {
	this->player = player;
}

String* Physical::toMultiLineString() {
	return menuString;
}

void Physical::action(short id, ActionParameter* parameter) {

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
