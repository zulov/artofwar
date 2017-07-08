#include "Entity.h"
#include "ObjectEnums.h"
#include "Game.h"


Entity::Entity(Vector3* _position, Urho3D::Node* _boxNode) {
	node = _boxNode;
	position = _position;
	rotation = new Vector3();
	alive = true;
	for (int i = 0; i < BUCKET_SET_NUMBER; ++i) {
		bucketZ[i] = bucketX[i] = INT_MIN;
	}
	healthBar = node->CreateChild();
	double healthBarSize = getHealthBarSize();

	StaticModel* model = node->GetComponent<StaticModel>();
	if (model) {
		Model* model3d = model->GetModel();//TODO razy scale?
		Vector3 boundingBox = model3d->GetBoundingBox().Size();

		healthBar->SetPosition(Vector3(0, boundingBox.y_ * 1.3f, 0));

		billboardSetBar = healthBar->CreateComponent<BillboardSet>();
		billboardSetBar->SetNumBillboards(1);
		billboardSetBar->SetMaterial(Game::get()->getCache()->GetResource<Material>("Materials/red.xml"));
		billboardSetBar->SetSorted(true);

		billboard = billboardSetBar->GetBillboard(0);
		billboard->size_ = Vector2(healthBarSize, 0.2);
		billboard->enabled_ = false;

		selectShadow = node->CreateChild();
		selectShadow->SetPosition(Vector3(0, 0.1, 0));
		selectShadow->Rotate(Quaternion(90, 0, 0));
		billboardSetShadow = selectShadow->CreateComponent<BillboardSet>();
		billboardSetShadow->SetNumBillboards(1);
		billboardSetShadow->SetMaterial(Game::get()->getCache()->GetResource<Material>("Materials/select.xml"));
		billboardSetShadow->SetSorted(true);
		billboardSetShadow->SetFaceCameraMode(FaceCameraMode::FC_NONE);

		billboardShadow = billboardSetShadow->GetBillboard(0);
		billboardShadow->size_ = Vector2(boundingBox.x_ * 1.3f, boundingBox.z_ * 1.3f);

		billboardShadow->enabled_ = false;

		billboardSetBar->Commit();
		billboardSetShadow->Commit();
	}

}

Entity::~Entity() {
	delete position;
	delete rotation;
}

void Entity::updateHealthBar() {
	double healthBarSize = getHealthBarSize();

	billboard->size_ = Vector2(healthBarSize, 0.2);
	billboardSetBar->Commit();
	billboardSetShadow->Commit();
}

double Entity::getHealthBarSize() {
	return 1;
}

double Entity::getMinimalDistance() {
	return minimalDistance;
}

Urho3D::Node* Entity::getNode() {
	return node;
}

signed char Entity::getBucketX(signed char param) {
	return bucketX[param];
}

signed char Entity::getBucketZ(signed char param) {
	return bucketZ[param];
}

bool Entity::isAlive() {
	return alive;
}

bool Entity::bucketHasChanged(short int posX, short int posZ, short int param) {
	if (bucketX[param] == posX && bucketZ[param] == posZ) {
		return false;
	}
	return true;

}

void Entity::setBucket(short int posX, short int posZ, short int param) {
	bucketX[param] = posX;
	bucketZ[param] = posZ;
}

void Entity::setTeam(signed char _team) {
	team = _team;
}

void Entity::setPlayer(signed char player) {
	this->player = player;
}

ObjectType Entity::getType() {
	return ENTITY;
}

int Entity::getSubType() {
	return -1;
}

void Entity::action(ActionType actionType, ActionParameter* parameter) {
}

signed char Entity::getTeam() {
	return team;
}

void Entity::absorbAttack(double attackCoef) {

}

Urho3D::Vector3* Entity::getPosition() {
	return position;
}


void Entity::select() {
	billboard->enabled_ = true;
	billboardShadow->enabled_ = true;
	updateHealthBar();
}

void Entity::unSelect() {
	billboard->enabled_ = false;
	billboardShadow->enabled_ = false;

	billboardSetBar->Commit();
	billboardSetShadow->Commit();
}
