#pragma once
#include <optional>
#include <span>
#include <string>
#include <vector>
#include "objects/ObjectEnums.h"
#include "player/Possession.h"
#include "Urho3D/Math/Vector3.h"


class SelectedObject;

namespace Urho3D
{
	class Node;
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
	short getId() const { return id; }

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
	virtual bool isIndexSlotOccupied(int indexToInteract) { return false; }
	virtual bool isFirstThingInSameSocket() const { return true; }

	virtual void setOccupiedIndexSlot(unsigned char index, bool value) { }

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

	float getHealthPercent() const { return hp * invMaxHp; }
	signed char getTeam() const { return team; }
	Urho3D::Vector3& getPosition() { return position; }

	virtual char getPlayer() const { return player; }

	int getMainBucketIndex() const { return indexInGrid; }

	virtual void populate() { }

	virtual bool isToDispose() const { return false; }

	virtual std::vector<int> getIndexesForUse(Unit* user) = 0;
	virtual std::optional<std::tuple<Urho3D::Vector2, float>> getPosToUseWithDist(Unit* user) = 0;
	std::optional<Urho3D::Vector2> getPosToUseBy(Unit* follower);

	virtual float getMaxHpBarSize() const = 0;

	virtual std::pair<float, bool> absorbAttack(float attackCoef) = 0;

	void select(SelectedObject* selectedObject);
	virtual short getCostSum() const = 0;

	void unSelect();

	virtual float getSightRadius() const { return -1.f; }
	virtual Urho3D::String toMultiLineString();

	virtual char getLevelNum();

	virtual void addValues(std::span<float> vals) const { }

	virtual unsigned char getMaxRangeUsers() const { return 8; }
	virtual unsigned char getMaxCloseUsers() const { return 8; }

protected:
	void loadXml(const Urho3D::String& xmlName);
	void setPlayerAndTeam(int player);
	virtual float getHealthBarThick() const { return 0.12f; }
	Urho3D::Node* node{};
	Urho3D::StaticModel* model{};

	SelectedObject* selectedObject{};
	Urho3D::Vector3 position;

	char team, player = -1;

	unsigned char closeUsers = 0,
	              rangeUsers = 0;
	bool indexHasChanged = false;

	bool isVisible = false;

	short id = -1; // optm
	float invMaxHp; // optm
	float hp;
private:
	virtual float getAuraSize(const Urho3D::Vector3& boundingBox) const;

	int indexInGrid = -1;
};
