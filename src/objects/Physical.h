#pragma once
#include "Entity.h"
#include <Urho3D/Graphics/BillboardSet.h>

struct ActionParameter;

class Physical :
	public Entity
{
public:
	Physical(Urho3D::Vector3* _position, ObjectType _type);
	virtual ~Physical();

	void updateHealthBar();
	float getHealthBarSize();

	bool bucketHasChanged(int _bucketIndex) const;
	void setBucket(int _bucketIndex);

	void updateBillbords() const;
	void initBillbords();

	void setTeam(unsigned char _team);
	void setPlayer(unsigned char player);

	static std::string getColumns();
	std::string getValues(int precision) override;
	bool isFirstThingAlive();
	bool hasEnemy();

	virtual bool belowCloseLimit();
	void reduceClose() { --closeUsers; }
	void upClose() { ++closeUsers; }

	bool belowRangeLimit() const { return rangeUsers < maxRangeUsers; }
	void reduceRange() { --rangeUsers; }
	void upRange() { ++rangeUsers; }

	virtual float getHealthPercent() const { return hpCoef / maxHpCoef; }
	signed char getTeam() const { return team; }
	Urho3D::Vector3* getPosition() const { return position; }
	unsigned char getPlayer() const { return player; }
	int getBucketIndex() const { return indexInGrid; }//TODO zrobic funkcje get index in mainGRID 
	virtual int getMainCell() const;

	virtual bool isToDispose() const;
	virtual Urho3D::Vector2 getPosToFollow(Urho3D::Vector3* center) const;
	virtual float getMaxHpBarSize();
	virtual void absorbAttack(float attackCoef);
	virtual void select();
	virtual void unSelect();
	virtual Urho3D::String& toMultiLineString();
	virtual void action(char id, ActionParameter& parameter);
	virtual int getLevel();
	virtual void clean();
protected:
	Urho3D::Vector3* position = nullptr;
	Urho3D::String menuString = "";

	std::vector<Physical*> thingsToInteract; //TODO jak to wczytac :O

	unsigned char team;
	unsigned char player;

	float hpCoef = 100;
	float maxHpCoef = 100;
	float attackCoef = 10;
	float attackRange;
	float defenseCoef = 0.3f;
	float attackSpeed = 1;
	float attackProccess = 0;

	Urho3D::Node* billboardNode = nullptr;
	Urho3D::Node* barNode = nullptr;
	Urho3D::Billboard* billboardBar = nullptr;
	Urho3D::Billboard* billboardShadow = nullptr;
	Urho3D::BillboardSet* billboardSetBar = nullptr;
	Urho3D::BillboardSet* billboardSetShadow = nullptr;

	unsigned char maxCloseUsers = 4; //TODO default values
	unsigned char maxRangeUsers = 2;

	unsigned char closeUsers = 0;
	unsigned char rangeUsers = 0;
private:
	void createBillboardBar();
	void updateBillboardBar(Urho3D::Vector3& boundingBox) const;
	void createBillboardShadow();
	void updateBillboardShadow(Urho3D::Vector3& boundingBox) const;

	int indexInGrid;
};
