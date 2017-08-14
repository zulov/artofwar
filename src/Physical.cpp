#include "Physical.h"
#include "Game.h"
#include <Urho3D/Graphics/Octree.h>

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
		billboardBar->size_ = Vector2(2, 0.2);
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
}

Physical::~Physical() {
	delete position;
	delete rotation;
}


void Physical::updateHealthBar() {
	double healthBarSize = getHealthBarSize();

	billboardBar->size_ = Vector2(healthBarSize, 0.2) / node->GetScale2D();
	billboardSetBar->Commit();
}

double Physical::getHealthBarSize() {
	return 1;
}

double Physical::getMinimalDistance() {
	return minimalDistance;
}

signed char Physical::getBucketX(signed char param) {
	return bucketX[param];
}

signed char Physical::getBucketZ(signed char param) {
	return bucketZ[param];
}

bool Physical::bucketHasChanged(short int posX, short int posZ, short int param) {
	if (bucketX[param] == posX && bucketZ[param] == posZ) {
		return false;
	}
	return true;
}

void Physical::setBucket(short int posX, short int posZ, short int param) {
	bucketX[param] = posX;
	bucketZ[param] = posZ;
}

void Physical::setTeam(signed char _team) {
	team = _team;
}

void Physical::setPlayer(signed char player) {
	this->player = player;
}

String Physical::toMultiLineString() {
	return "Physical";
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
