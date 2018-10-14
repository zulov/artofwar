#pragma once
#include "Entity.h"
#include "scene/load/dbload_container.h"
#include <Urho3D/Graphics/BillboardSet.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <iostream>

struct ActionParameter;
class Unit;

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

	void updateBillboards() const;
	void initBillboards();

	void setTeam(unsigned char _team);
	void setPlayer(unsigned char player);
	bool isSelected() const;
	void load(dbload_physical* dbloadPhysical);
	virtual bool isSlotOccupied(int indexToInteract) { return false; }
	virtual void setOccupiedSlot(int indexToInteract, bool value){}

	static std::string getColumns();
	std::string getValues(int precision) override;
	virtual bool isUsable() const { return isAlive(); }

	virtual int belowCloseLimit();
	void reduceClose() { --closeUsers; }
	void upClose() { ++closeUsers; }

	bool belowRangeLimit() const { return rangeUsers < maxRangeUsers; }
	void reduceRange() { --rangeUsers; }
	void upRange() { ++rangeUsers; }

	virtual float getHealthPercent() const { return hpCoef / maxHpCoef; }
	signed char getTeam() const { return team; }
	Urho3D::Vector3* getPosition() const { return position; }

	virtual char getPlayer() const { return player; }

	int getBucketIndex() const { return indexInGrid; }

	virtual void populate() {
	};
	virtual int getMainCell() const;

	virtual bool isToDispose() const { return false; }
	virtual std::tuple<Urho3D::Vector2, int> getPosToUseWithIndex(Unit* follower) const;

	virtual Urho3D::Vector2 getPosToUse(Unit* follower) const;

	virtual float getMaxHpBarSize() { return 0; }

	virtual void absorbAttack(float attackCoef) {
	}

	virtual void select();
	virtual void unSelect();
	virtual Urho3D::String& toMultiLineString();

	virtual void action(char id, const ActionParameter& parameter) {
	}

	virtual int getLevel();

	virtual void clean() {
	}

protected:
	void loadXml(Urho3D::String xmlName);
	void setPlayerAndTeam(int player);
	virtual float getHealthBarThick() { return 0.15; }
	Urho3D::Vector3* position = nullptr;
	Urho3D::String menuString = "";

	char team;
	char player;

	float hpCoef = 100;
	float maxHpCoef = 100;
	float attackCoef = 10;
	float attackRange;
	float defenseCoef = 0.3f;
	float attackSpeed = 1;
	float attackProcess = 0;

	Urho3D::StaticModel* model;

	Urho3D::Node *billboardNode, *barNode;
	Urho3D::Billboard *billboardBar, *billboardShadow;
	Urho3D::BillboardSet *billboardSetBar, *billboardSetShadow;

	unsigned char maxCloseUsers = 4; //TODO default values
	unsigned char maxRangeUsers = 8;

	unsigned char closeUsers = 0;
	unsigned char rangeUsers = 0;
private:
	void createBillboardBar();
	void updateBillboardBar(Urho3D::Vector3& boundingBox) const;
	Urho3D::Billboard* createBillboardSet(Urho3D::Node*& node, Urho3D::BillboardSet*& billbordSet, const Urho3D::String&
	                                      material) const;
	void createBillboardShadow();
	void updateBillboardShadow(Urho3D::Vector3& boundingBox) const;

	int indexInGrid;
};
