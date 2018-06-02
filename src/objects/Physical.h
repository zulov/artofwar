#pragma once
#include "Entity.h"
#include <Urho3D/Graphics/BillboardSet.h>

#define BUCKET_SET_NUMBER 4

struct ActionParameter;

class Physical :
	public Entity
{
public:
	Physical(Vector3* _position, ObjectType _type);
	virtual ~Physical();

	void updateHealthBar();
	float getHealthBarSize();
	bool bucketHasChanged(int _bucketIndex, char param);
	void setBucket(int _bucketIndex, char param);
	void updateBillbords() const;
	void initBillbords();

	void setTeam(unsigned char _team);
	void setPlayer(unsigned char player);
	static std::string getColumns();
	std::string getValues(int precision) override;
	bool isFirstThingAlive();
	bool hasEnemy();

	bool belowCloseLimit() const { return closeUsers < maxCloseUsers; }
	void reduceClose() { --closeUsers; }
	void upClose() { ++closeUsers; }

	bool belowRangeLimit() const { return rangeUsers < maxRangeUsers; }
	void reduceRange() { --rangeUsers; }
	void upRange() { ++rangeUsers; }

	float getHealthPercent() const { return hpCoef / maxHpCoef; }
	signed char getTeam() const { return team; }
	Vector3* getPosition() const { return position; }
	unsigned char getPlayer() const { return player; }
	int getBucketIndex(char param) const { return bucketIndexShift[param]; }

	virtual float getMaxHpBarSize();
	virtual void absorbAttack(float attackCoef);
	virtual void select();
	virtual void unSelect();
	virtual String& toMultiLineString();
	virtual void action(char id, ActionParameter& parameter);
	virtual int getLevel();
	virtual void clean();
protected:
	Vector3* position = nullptr;
	String menuString = "";

	std::vector<Physical*> thingsToInteract;

	unsigned char team;
	unsigned char player;

	float hpCoef = 100;
	float maxHpCoef = 100;
	float attackCoef = 10;
	float attackRange;
	float defenseCoef = 0.3f;
	float attackSpeed = 1;
	float attackProccess = 0;

	Node* billboardNode = nullptr;
	Node* barNode = nullptr;
	Billboard* billboardBar = nullptr;
	Billboard* billboardShadow = nullptr;
	BillboardSet* billboardSetBar = nullptr;
	BillboardSet* billboardSetShadow = nullptr;

	unsigned char maxCloseUsers = 4; //TODO default values
	unsigned char maxRangeUsers = 2;

	unsigned char closeUsers = 0;
	unsigned char rangeUsers = 0;
private:
	void createBillboardBar();
	void updateBillboardBar(Vector3& boundingBox) const;
	void createBillboardShadow();
	void updateBillboardShadow(Vector3& boundingBox) const;

	int bucketIndex[BUCKET_SET_NUMBER];
	int* bucketIndexShift;
};
