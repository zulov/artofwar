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
	int getBucketIndex(char param);
	bool bucketHasChanged(int _bucketIndex, char param);
	void setBucket(int _bucketIndex, char param);
	signed char getTeam();
	virtual void absorbAttack(double attackCoef);
	virtual void select();
	virtual void unSelect();
	Vector3* getPosition();

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

	Node* billboardNode;
	Node* barNode;
	Billboard* billboardBar;
	Billboard* billboardShadow;
	BillboardSet* billboardSetBar;
	BillboardSet* billboardSetShadow;

private:

	int bucketIndex[BUCKET_SET_NUMBER];
};
