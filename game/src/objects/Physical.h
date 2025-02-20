#pragma once
#include <optional>
#include <span>
#include <string>
#include <vector>
#include <Urho3D/Graphics/Material.h>

#include "UId.h"
#include "database/db_other_struct.h"
#include "objects/ObjectEnums.h"
#include "Urho3D/Math/Vector3.h"


enum class VisibilityType : char;

namespace Urho3D {
	class Node;
	class StaticModel;
}

class Unit;
struct dbload_physical;

class Physical {
	friend class CreationCommand;
public:
	explicit Physical(Urho3D::Vector3& _position, UId uId);
	virtual ~Physical();
	void clearNodeWithOutDelete();

	virtual bool isAlive() const;
	short getDbId() const { return dbId; }
	virtual char getSecondaryId() const { return -1; }

	virtual float getHealthBarSize() const;

	virtual void setBucketInMainGrid(int _bucketIndex) {
		assert(_bucketIndex >= 0);
		indexInMainGrid = _bucketIndex;
	}

	int getMainGridIndex() const { return indexInMainGrid; }

	virtual ObjectType getType() const = 0;

	signed char getTeam() const { return team; }
	virtual char getPlayer() const { return player; }

	void load(dbload_physical* dbloadPhysical);
	virtual bool indexChanged() const { return false; }

	virtual void setOccupiedIndexSlot(char index, bool value) { }

	virtual std::string getValues(int precision);
	virtual bool isUsable() const { return isAlive(); }

	virtual int belowCloseLimit() const;
	void reduceClose() { --closeUsers; }
	void upClose() { ++closeUsers; }

	bool belowRangeLimit() const { return rangeUsers < getMaxRangeUsers(); }
	void reduceRange() { --rangeUsers; }
	void upRange() { ++rangeUsers; }

	virtual float getAttackVal(Physical* aim) { return 0.f; }

	float getHealthPercent() const { return hp * invMaxHp; }
	const Urho3D::Vector3& getPosition() const { return position; }

	virtual bool isToDispose() const { return false; }

	virtual std::vector<int> getIndexesForUse() const = 0;
	virtual std::vector<int> getIndexesForRangeUse(Unit* user) const = 0;
	virtual void addIndexesForUse(std::vector<int>& indexes) const = 0;
	virtual bool indexCanBeUse(int index) const = 0;

	virtual std::optional<std::tuple<Urho3D::Vector2, float>> getPosToUseWithDist(Unit* user) = 0;
	std::optional<Urho3D::Vector2> getPosToUseBy(Unit* follower);

	virtual unsigned short getMaxHpBarSize() const = 0;

	virtual std::pair<float, bool> absorbAttack(float attackCoef) = 0;

	bool isSelected() const;
	void select();
	void clearSelection();
	void unSelect();

	virtual short getCostSum() const = 0;

	virtual float getSightRadius() const { return -1.f; }
	virtual Urho3D::String getInfo() const = 0;

	virtual char getLevelNum();

	virtual void addValues(std::span<float> vals) const { }

	virtual unsigned char getMaxRangeUsers() const { return 128; }
	virtual unsigned char getMaxCloseUsers() const { return 8; }
	virtual bool isInCloseRange(int index) const = 0;
	virtual const Urho3D::String& getName() const = 0;
	virtual float getModelHeight() const =0;
	virtual void setVisibility(VisibilityType type) = 0;
	virtual unsigned char getHealthBarThick() const { return 5; }
	bool isNodeEnabled() const;
	void setDefaultShader(Urho3D::Material* mat) const;
	void ensureMaterialCloned();
	int getIndexInInfluence() const { return indexInInfluence; }
	void setIndexInInfluence(int index);
	Urho3D::Node* getNode() const { return node; }
	float getHp() const { return hp; }

protected:
	virtual void setModelData(float modelHeight) const =0;

	virtual Urho3D::Color getColor(db_player_colors* col) const = 0;
	void loadXml(const Urho3D::String& xmlName);
	void setPlayerAndTeam(char playerId, char teamId);

	Urho3D::Node* node{};
	Urho3D::Vector3 position;

	float hp = -1.f;
	float invMaxHp; // optm

	int indexInInfluence = -1;
	int indexInMainGrid = -1;
	UId uId;
	short dbId = -1; // optm

	char team, player = -1;

	unsigned char closeUsers = 0,
	              rangeUsers = 0;

	bool selected = false;
	bool materialCloned = false;
};
