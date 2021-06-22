#pragma once
#include <optional>
#include <span>
#include <string>
#include <vector>


#include "objects/ObjectEnums.h"
#include "Urho3D/Math/Vector3.h"


namespace Urho3D {
	class Node;
	struct Billboard;
	class StaticModel;
}

class Unit;
struct dbload_physical;

class Physical {
public:
	explicit Physical(Urho3D::Vector3& _position, bool withNode);
	virtual ~Physical();
	void clearNodeWithOutDelete();

	virtual bool isAlive() const;
	virtual short getId();

	void updateHealthBar() const;
	virtual float getHealthBarSize() const;

	bool bucketHasChanged(int _bucketIndex) const { return indexInGrid != _bucketIndex; }

	void setBucket(int _bucketIndex) {
		indexInGrid = _bucketIndex;
		indexHasChanged = true;
	}

	void updateBillboards() const;

	void setTeam(unsigned char team) { this->team = team; }
	void setPlayer(unsigned char player) { this->player = player; }
	bool isSelected() const;
	void load(dbload_physical* dbloadPhysical);
	virtual bool isSlotOccupied(int indexToInteract) { return false; }
	virtual bool isFirstThingInSameSocket() const { return true; }

	virtual void setOccupiedSlot(unsigned char index, bool value) {
	}

	void indexHasChangedReset() { indexHasChanged = false; }

	virtual std::string getValues(int precision);
	virtual bool isUsable() const { return isAlive(); }

	virtual int belowCloseLimit();
	void reduceClose() { --closeUsers; }
	void upClose() { ++closeUsers; }

	bool belowRangeLimit() const { return rangeUsers < getMaxRangeUsers(); }
	void reduceRange() { --rangeUsers; }
	void upRange() { ++rangeUsers; }

	virtual ObjectType getType() const = 0;

	virtual float getInvMaxHp() const = 0;
	float getHealthPercent() const { return hp * getInvMaxHp(); }
	signed char getTeam() const { return team; }
	Urho3D::Vector3& getPosition() { return position; }

	virtual char getPlayer() const { return player; }

	int getMainBucketIndex() const { return indexInGrid; }

	virtual void populate() {
	}

	virtual bool isToDispose() const { return false; }

	virtual std::vector<int> getIndexesForUse(Unit* user) = 0;
	virtual std::optional<std::tuple<Urho3D::Vector2, float>> getPosToUseWithDist(Unit* user) = 0;
	std::optional<Urho3D::Vector2> getPosToUseBy(Unit* follower);

	virtual float getMaxHpBarSize() const = 0;

	virtual float absorbAttack(float attackCoef) { return 0.0f; }

	void select(Urho3D::Billboard* healthBar, Urho3D::Billboard* aura);
	static void disableBillboard(Urho3D::Billboard* billboard);
	void unSelect();

	virtual float getSightRadius() const { return -1.f; }
	virtual Urho3D::String toMultiLineString();

	virtual char getLevelNum();

	virtual void addValues(std::span<float> vals) const {
	}

	virtual unsigned char getMaxRangeUsers() const { return 8; }
	virtual unsigned char getMaxCloseUsers() const { return 8; }

protected:
	void loadXml(const Urho3D::String& xmlName);
	void setPlayerAndTeam(int player);
	virtual float getHealthBarThick() const { return 0.12f; }
	Urho3D::Node* node{};
	Urho3D::StaticModel* model{};

	Urho3D::Billboard *healthBar = nullptr, *aura = nullptr;

	char team, player = -1;

	unsigned char closeUsers = 0,
	              rangeUsers = 0;
	bool indexHasChanged = false;
	bool selected = false;
	bool isVisible = false;

	Urho3D::Vector3 position;

	float hp;
private:
	void updateBillboardBar(Urho3D::Vector3& boundingBox) const;
	void updateBillboardAura(Urho3D::Vector3& boundingBox) const;
	virtual float getAuraSize(const Urho3D::Vector3& boundingBox) const;

	int indexInGrid = -1;
};
