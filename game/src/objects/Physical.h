#pragma once
#include <optional>
#include <span>
#include <string>
#include <vector>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Graphics/Material.h>

#include "database/db_other_struct.h"
#include "objects/ObjectEnums.h"
#include "player/Possession.h"
#include "Urho3D/Math/Vector3.h"


enum class VisibilityType : char;

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

	virtual float getHealthBarSize() const;

	void setBucket(int _bucketIndex) {
		if (_bucketIndex < 0) {
			indexHasChanged = false;
		} else {
			indexInMainGrid = _bucketIndex;
			indexHasChanged = true;
		}
	}

	void setTeam(unsigned char team) { this->team = team; }
	void setPlayer(unsigned char player) { this->player = player; }
	bool isSelected() const;
	void load(dbload_physical* dbloadPhysical);
	virtual bool isIndexSlotOccupied(int indexToInteract) { return false; }
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

	virtual ObjectType getType() const = 0;
	virtual float getAttackVal(Physical* aim) { return 0.f; }

	float getHealthPercent() const { return hp * invMaxHp; }
	signed char getTeam() const { return team; }
	const Urho3D::Vector3& getPosition() const { return position; }

	virtual char getPlayer() const { return player; }

	int getMainGridIndex() const { return indexInMainGrid; }

	virtual void populate() { }

	virtual bool isToDispose() const { return false; }

	virtual std::vector<int> getIndexesForUse(Unit* user) const = 0;
	virtual std::vector<int> getIndexesForRangeUse(Unit* user) const = 0;
	virtual std::optional<std::tuple<Urho3D::Vector2, float>> getPosToUseWithDist(Unit* user) = 0;
	std::optional<Urho3D::Vector2> getPosToUseBy(Unit* follower);

	virtual unsigned short getMaxHpBarSize() const = 0;

	virtual std::pair<float, bool> absorbAttack(float attackCoef) = 0;

	void select();
	void clearSelection();
	virtual short getCostSum() const = 0;

	void unSelect();

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
	Urho3D::Node* getNode() { return node; }
protected:
	virtual void setModelData(float modelHeight) const =0;

	virtual Urho3D::Color getColor(db_player_colors* col) const = 0;
	void loadXml(const Urho3D::String& xmlName);
	void setPlayerAndTeam(int player);

	Urho3D::Node* node{};
	Urho3D::Vector3 position;

	int indexInMainGrid = -1;

	float hp = -1;
	float invMaxHp; // optm
	short id = -1; // optm

	char team, player = -1;

	unsigned char closeUsers = 0,
	              rangeUsers = 0;
	bool indexHasChanged = false; ///TODO to tylko dla unit

	bool shouldUpdate = false;

	bool selected = false;
	bool materialCloned = false;
};
