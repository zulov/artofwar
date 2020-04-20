#pragma once

#include "aim/Aims.h"
#include "objects/Physical.h"
#include "state/UnitState.h"
#include <Urho3D/Graphics/CustomGeometry.h>
#include <vector>


#define BUCKET_SET_NUMBER 4
#define USE_SOCKETS_NUMBER 8

enum class UnitAction : char;
enum class DebugUnitType : char;
enum class SimColorMode : char;
struct ForceStats;
struct db_unit_level;
struct db_unit;
struct dbload_unit;
class FutureOrder;
struct MissileData;
struct ChargeData;

class Aim;
class State;
class ResourceEntity;

class Unit : public Physical {
	friend class State;
	friend class StopState;
	friend class DefendState;
	friend class DeadState;
	friend class GoState;
	friend class PatrolState;
	friend class FollowState;
	friend class AttackState;
	friend class ChargeState;
	friend class CollectState;
	friend class DisposeState;
	friend class ShotState;
	friend class MoveState;
public:
	Unit(Urho3D::Vector3& _position, int id, int player, int level);
	~Unit();

	void populate() override;
	void checkAim();
	void move(double timeStep);
	void applyForce(double timeStep);
	void setAcceleration(Urho3D::Vector2& _acceleration);

	void forceGo(float boostCoef, float aimCoef, Urho3D::Vector2& force) const;
	Urho3D::Vector2 getDestination(float boostCoef, float aimCoef);

	void absorbAttack(float attackCoef) override;

	void toCharge(std::vector<Physical*>* enemies);

	void toAction(Physical* closest, float minDistance, int indexToInteract, UnitAction order);
	void toAction(Physical* closest, float minDistance, int indexToInteract, UnitAction order,
	              float attackInterest);

	void updateHeight(float y, double timeStep);

	void setState(UnitState _state);

	void load(dbload_unit* unit);

	ObjectType getType() const override;
	void setFormation(short _formation);
	void resetFormation();
	void setPositionInFormation(short _pos);
	void clearAims();
	void removeCurrentAim();
	void setIndexToInteract(int index);
	int getIndexToInteract() const { return indexToInteract; }
	bool closeEnoughToAttack();
	bool isInRightSocket() const;

	static std::string getColumns();
	void changeColor(SimColorMode mode);
	void addOrder(FutureOrder* aim);

	void drawLineTo(const Urho3D::Vector3& second, const Urho3D::Color& color) const;
	void debug(DebugUnitType type, ForceStats& stats);
	bool isFirstThingAlive() const;

	std::tuple<Physical*, float, int> closestPhysical(std::vector<Physical*>* things,
	                                                  const std::function<bool(Physical*)>& condition);

	float getMaxSeparationDistance() const { return maxSeparationDistance; }
	short getPositionInFormation() const { return posInFormation; }
	float getMinimalDistance() const { return minimalDistance; }
	UnitState getActionState() const { return actionState; }
	UnitState getState() const { return state; }
	short getFormation() const { return formation; }
	bool isToDispose() const override { return state == UnitState::DISPOSE && atState; }
	bool hasAim() const { return aims.hasAim(); }

	float getAttackRange() const;
	void setBucket(int _bucketIndex) override;
	bool bucketHasChanged(int _bucketIndex, char param) const;
	int getBucketIndex(char param) const { return teamBucketIndex[param]; }
	void setBucket(int _bucketIndex, char param);
	bool isSlotOccupied(int indexToInteract) override;
	void setOccupiedSlot(int indexToInteract, bool value) override { useSockets[indexToInteract] = value; }
	Urho3D::Vector2 getPosToUse();

	std::optional<std::tuple<Urho3D::Vector2, float, int>> getPosToUseWithIndex(Unit* follower) override;

	void action(UnitAction unitAction, const ActionParameter& parameter);
	void action(UnitAction unitAction);

	bool isFirstThingInSameSocket() const override;
	std::string getValues(int precision) override;
	Urho3D::String toMultiLineString() override;
	float getMaxHpBarSize() override;
	bool isAlive() const override;
	int getLevel() override;
	short getId() override;
	void clean() override;
	float getValueOf(ValueType type) const override;
	Urho3D::Vector2 getSocketPos(Unit* toFollow, int i) const;
private:
	void actionIfCloseEnough(UnitAction order, Physical* closest, int indexToInteract,
	                         float sqDistance, float closeRange, float interestRange);
	void changeColor(float value, float maxValue) const;
	void setAim(Aim* aim);

	UnitState state, actionState;
	bool atState = false, rotatable;

	Urho3D::Vector2 velocity, acceleration;
	Aims aims;

	db_unit* dbUnit;
	db_unit_level* dbLevel;

	Urho3D::Material* basic;

	ChargeData* chargeData{};
	MissileData* missileData{};

	std::vector<Physical*> thingsToInteract; //TODO jak to wczytac :O

	float minimalDistance, maxSeparationDistance,
	      maxSpeed, minSpeed,
	      attackInterest, collectSpeed;

	short posInFormation = -1, formation = -1;
	unsigned short currentFrameState = 0;

	int indexToInteract = -1;
	bool useSockets[USE_SOCKETS_NUMBER];
	int teamBucketIndex[BUCKET_SET_NUMBER];
};
