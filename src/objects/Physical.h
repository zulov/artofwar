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

	void updateHealthBar() const;
	float getHealthBarSize() const;

	bool bucketHasChanged(int _bucketIndex) const;
	virtual void setBucket(int _bucketIndex);

	void updateBillboards() const;

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

	virtual float getMaxHpBarSize() const { return 0; }

	virtual void absorbAttack(float attackCoef) {
	}

	virtual void select(Urho3D::Billboard* billboardBar1, Urho3D::Billboard* billboardShadow1);
	virtual void unSelect();
	virtual Urho3D::String toMultiLineString();

	virtual int getLevel();

	virtual void clean() {
	}

	virtual float getValueOf(ValueType type) const;
	virtual void fillValues(std::span<float> weights) const;
	virtual void addValues(std::span<float> vals) const;
protected:
	void loadXml(const Urho3D::String& xmlName);
	void setPlayerAndTeam(int player);
	virtual float getHealthBarThick() const { return 0.12; }
	Urho3D::Node* node;

	Urho3D::StaticModel* model;

	Urho3D::Billboard *billboardBar1 = nullptr, *billboardShadow1 = nullptr;

	char team, player = -1;
	unsigned char maxRangeUsers = 8,
	              maxCloseUsers = 8, //TODO default values
	              closeUsers = 0,
	              rangeUsers = 0;
	bool indexHasChanged = false;
	bool selected = false;

	Urho3D::Vector3 position;

	float hp = 100, maxHp = 100,
	      attackCoef = 10, attackSpeed = 1,
	      defenseCoef = 0.3f;

private:
	void updateBillboardBar(Urho3D::Vector3& boundingBox) const;
	void updateBillboardShadow(Urho3D::Vector3& boundingBox) const;
	virtual float getShadowSize(const Urho3D::Vector3& boundingBox) const;


	int indexInGrid;
};
