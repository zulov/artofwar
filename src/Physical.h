#pragma once
#include "Entity.h"
#include <Urho3D/Graphics/BillboardSet.h>
#include "defines.h"

class Physical :
	public Entity
{
public:
	Physical(Vector3* _position, Urho3D::Node* _node, ObjectType _type);
	~Physical();
	void updateHealthBar();
	virtual double getHealthBarSize();
	signed char getTeam();
	virtual void absorbAttack(double attackCoef);
	virtual void select();
	virtual void unSelect();
	Vector3* getPosition();
	double getMinimalDistance();
	signed char getBucketX(signed char param);
	signed char getBucketZ(signed char param);
	bool bucketHasChanged(short posX, short posY, short param);
	void setBucket(short posX, short posY, short param);
	void setTeam(signed char _team);
	void setPlayer(signed char player);
	virtual String* toMultiLineString();//TODO optimize create only if change and pointer?
	virtual void buttonAction(short id);
protected:
	Vector3* position;
	Vector3* rotation;
	double minimalDistance;
	signed char team;
	signed char player;
	bool rotatable;
	double hpCoef = 100;
	double maxHpCoef = 100;
	double attackCoef = 10;
	double attackRange;
	double defenseCoef = 0.3;
	double attackSpeed = 1;

	String textureName;

	Node* billboardNode;
	Node* barNode;
	Billboard* billboardBar;
	Billboard* billboardShadow;
	BillboardSet* billboardSetBar;
	BillboardSet* billboardSetShadow;
	String* menuString;
private:

	short int bucketX[BUCKET_SET_NUMBER];
	short int bucketZ[BUCKET_SET_NUMBER];
};
