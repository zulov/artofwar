#pragma once
#include "scene/load/dbload_container.h"
#include "objects/ObjectEnums.h"
#include <Urho3D/Graphics/BillboardSet.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <iostream>
#include <optional>
#include <span>

enum class ValueType : char;
struct ActionParameter;
class Unit;

class Physical {
public:
	explicit Physical(Urho3D::Vector3& _position);
	virtual ~Physical();

	virtual bool isAlive() const;
	virtual short getId();
	Urho3D::Node* getNode() const { return node; }

	void updateHealthBar();
	float getHealthBarSize();

	bool bucketHasChanged(int _bucketIndex) const;
	virtual void setBucket(int _bucketIndex);

	void updateBillboards() const;
	void initBillboards();

	void setTeam(unsigned char _team);
	void setPlayer(unsigned char player);
	bool isSelected() const;
	void load(dbload_physical* dbloadPhysical);
	virtual bool isSlotOccupied(int indexToInteract) { return false; }
	virtual bool isFirstThingInSameSocket() const { return true; }

	virtual void setOccupiedSlot(int indexToInteract, bool value) {
	}

	void indexHasChangedReset();

	static std::string getColumns();
	virtual std::string getValues(int precision);
	virtual bool isUsable() const { return isAlive(); }

	virtual int belowCloseLimit();
	void reduceClose() { --closeUsers; }
	void upClose() { ++closeUsers; }

	bool belowRangeLimit() const { return rangeUsers < maxRangeUsers; }
	void reduceRange() { --rangeUsers; }
	void upRange() { ++rangeUsers; }

	virtual ObjectType getType() const;

	virtual float getHealthPercent() const { return hp / maxHp; }
	signed char getTeam() const { return team; }
	Urho3D::Vector3& getPosition() { return position; }

	virtual char getPlayer() const { return player; }
	virtual std::optional<int> getDeploy() { return {}; }

	int getMainBucketIndex() const { return indexInGrid; }

	virtual void populate() {
	}

	virtual int getMainCell() const;

	virtual bool isToDispose() const { return false; }
	virtual std::optional<std::tuple<Urho3D::Vector2, float, int>> getPosToUseWithIndex(Unit* follower);

	virtual std::optional<Urho3D::Vector2> getPosToUseBy(Unit* follower);

	virtual float getMaxHpBarSize() { return 0; }

	virtual void absorbAttack(float attackCoef) {
	}

	virtual void select();
	virtual void unSelect();
	virtual Urho3D::String toMultiLineString();

	virtual int getLevel();

	virtual void clean() {
	}

	virtual float getValueOf(ValueType type) const;
	virtual void fillValues(std::span<float> weights) const;
protected:
	void loadXml(const Urho3D::String& xmlName);
	void setPlayerAndTeam(int player);
	virtual float getHealthBarThick() { return 0.15; }
	Urho3D::Node* node;

	Urho3D::StaticModel* model;

	Urho3D::Node *billboardNode, *barNode;
	Urho3D::Billboard *billboardBar, *billboardShadow;
	Urho3D::BillboardSet *billboardSetBar, *billboardSetShadow;

	char team, player = -1;
	unsigned char maxRangeUsers = 8,
	              maxCloseUsers = 8, //TODO default values
	              closeUsers = 0,
	              rangeUsers = 0;
	bool indexHasChanged = false;

	Urho3D::Vector3 position;

	float hp = 100, maxHp = 100,
	      attackCoef = 10, attackSpeed = 1,
	      defenseCoef = 0.3f;
	
private:
	void createBillboardBar();
	void updateBillboardBar(Urho3D::Vector3& boundingBox) const;
	Urho3D::Billboard* createBillboardSet(Urho3D::Node*& node, Urho3D::BillboardSet*& billbordSet,
	                                      const Urho3D::String& material) const;
	void createBillboardShadow();
	void updateBillboardShadow(Urho3D::Vector3& boundingBox) const;
	virtual float getShadowSize(const Urho3D::Vector3& boundingBox) const;
	virtual Urho3D::String getBarMaterialName();
	virtual Urho3D::String getShadowMaterialName();


	int indexInGrid;
};
