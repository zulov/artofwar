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

	short getBucketX(char param);
	short getBucketZ(char param);
	bool bucketHasChanged(short posX, short posY, char param);
	void setBucket(short posX, short posY, char param);
	void setTeam(char _team);
	void setPlayer(char player);
	virtual String* toMultiLineString();
	virtual void action(short id, ActionParameter* parameter);

	bool hasEnemy();
	virtual void clean();
protected:
	Vector3* position;
	Vector3* rotation;
	String* menuString;

	Physical* enemyToAttack = nullptr;

	signed char team;
	signed char player;

	double hpCoef = 100;
	double maxHpCoef = 100;
	double attackCoef = 10;
	double attackRange;
	double defenseCoef = 0.3;
	double attackSpeed = 1;
	//String textureName;

	Node* billboardNode;
	Node* barNode;
	Billboard* billboardBar;
	Billboard* billboardShadow;
	BillboardSet* billboardSetBar;
	BillboardSet* billboardSetShadow;

private:

	short bucketX[BUCKET_SET_NUMBER];
	short bucketZ[BUCKET_SET_NUMBER];
};
