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
	Entity(Vector3* _position, Urho3D::Node* _node);
	virtual ~Entity();
	Vector3* getPosition();
	double getMinimalDistance();
	Urho3D::Node* getNode();
	signed char getBucketX(signed char param);
	signed char getBucketZ(signed char param);
	bool isAlive();
	bool bucketHasChanged(short int posX, short int posY, short int param);
	void setBucket(short int posX, short int posY, short int param);
	void setTeam(signed char _team);
	void setPlayer(signed char player);
	virtual ObjectType getType();
	virtual int getSubType();
	virtual void select();
	virtual void unSelect();
	virtual void action(ActionType actionType, ActionParameter* parameter);
	signed char getTeam();
	virtual void absorbAttack(double attackCoef);
protected:
	Urho3D::Node* node;
	Vector3* position;
	Vector3* rotation;
	double minimalDistance;
	signed char team;
	signed char player;
	bool rotatable;
	String textureName;
private:
	short int bucketX[BUCKET_SET_NUMBER];
	short int bucketZ[BUCKET_SET_NUMBER];
	bool alive;

};
