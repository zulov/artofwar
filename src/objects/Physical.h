#pragma once
#include "Entity.h"
#include "defines.h"
#include <Urho3D/Graphics/BillboardSet.h>


struct ActionParameter;

class Physical :
	public Entity
{
public:
	Physical(Vector3* _position, ObjectType _type);
	virtual ~Physical();
	void createBillboardBar();
	void updateBillboardBar(Vector3& boundingBox);
	void createBillboardShadow();
	void updateBillboardShadow(Vector3& boundingBox);
	void updateHealthBar();
	virtual float getHealthBarSize();
	virtual float getHealthPercent();
	int getBucketIndex(char param);
	bool bucketHasChanged(int _bucketIndex, char param);
	void setBucket(int _bucketIndex, char param);
	signed char getTeam();
	virtual void absorbAttack(double attackCoef);
	virtual void select();
	virtual void unSelect();
	Vector3* getPosition() { return position; }
	void updateBillbords();
	void initBillbords();

	void setTeam(unsigned char _team);
	void setPlayer(unsigned char player);
	unsigned char getPlayer();
	virtual String& toMultiLineString();
	virtual void action(short id, ActionParameter& parameter);
	static std::string getColumns();
	virtual int getLevel();
	std::string getValues(int precision) override;
	bool hasEnemy();
	virtual void clean();
protected:
	Vector3* position = nullptr;
	Vector3 rotation;
	String menuString = "";

	Physical* enemyToAttack = nullptr;

	unsigned char team;
	unsigned char player;

	float hpCoef = 100;
	float maxHpCoef = 100;
	float attackCoef = 10;
	float attackRange;
	float defenseCoef = 0.3f;
	float attackSpeed = 1;

	Node* billboardNode = nullptr;
	Node* barNode = nullptr;
	Billboard* billboardBar = nullptr;
	Billboard* billboardShadow = nullptr;
	BillboardSet* billboardSetBar = nullptr;
	BillboardSet* billboardSetShadow = nullptr;

private:

	int bucketIndex[BUCKET_SET_NUMBER];
	int* bucketIndexShift;
};
