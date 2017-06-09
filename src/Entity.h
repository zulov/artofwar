#pragma once
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Math/Vector3.h>
#include <Urho3D/UI/Text3D.h>
#include <Urho3D/Scene/Component.h>
#include "ActionType.h"
#include "Aims.h"
#include "ActionParameter.h"
#include "defines.h"
#include "ObjectEnums.h"

using namespace Urho3D;

class Entity
{
public:
	Entity(Vector3* _position, Urho3D::Node* _boxNode);
	virtual ~Entity();
	Vector3* getPosition();
	double getMinimalDistance();
	Urho3D::Node* getNode();
	int getBucketX(int param);
	int getBucketZ(int param);
	bool isAlive();
	bool bucketHasChanged(int posX, int posY, int param);
	void setBucket(int posX, int posY, int param);
	void setTeam(int _team);
	void setPlayer(int player);
	virtual ObjectType getType();
	virtual void select();
	virtual void unSelect();
	virtual void action(ActionType actionType, ActionParameter* parameter);
	int getTeam();
	virtual void absorbAttack(double attackCoef);
protected:
	Urho3D::Node* node;
	Vector3* position;
	Vector3* rotation;
	double minimalDistance;
	int team;
	int player;
	bool rotatable = true;
	String textureName;
private:
	int bucketX[BUCKET_SET_NUMBER];
	int bucketZ[BUCKET_SET_NUMBER];
	bool alive;

};
