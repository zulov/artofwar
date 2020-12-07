#pragma once

#include <Urho3D/Container/Str.h>
#include <Urho3D/Math/Vector2.h>
#include <functional>
#include <tuple>

#include "ActionParameter.h"
#include "aim/Aims.h"
#include "objects/ObjectEnums.h"
#include "objects/Physical.h"
#include "state/UnitState.h"

namespace Urho3D {
	class Vector4;
	class Color;
	class Material;
	class Vector3;
}

constexpr char BUCKET_SET_NUMBER = 2;
constexpr char USE_SOCKETS_NUMBER = 8;

enum class UnitAction : char;
enum class DebugUnitType : char;
enum class SimColorMode : char;
struct ForceStats;
struct db_unit_level;
struct db_unit;
struct dbload_unit;
struct MissileData;
struct ChargeData;
struct ActionParameter;

class UnitOrder;
class Aim;

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
	void updatePosition() const;
	bool move(float timeStep, const Urho3D::Vector4& boundary);
	bool ifVisible(const Urho3D::Vector4& boundary) const;
	void applyForce(float timeStep);
	void setAcceleration(Urho3D::Vector2& _acceleration);

	void forceGo(float boostCoef, float aimCoef, Urho3D::Vector2& force) const;
	Urho3D::Vector2 getDestination(float boostCoef, float aimCoef);

	float absorbAttack(float attackCoef) override;

	void toCharge(std::vector<Physical*>* enemies);

	void toAction(Physical* closest, float minDistance, UnitAction order, bool force = false);

	void updateHeight(float y, double timeStep);

	void setState(UnitState _state);

	void load(dbload_unit* unit);

	ObjectType getType() const override { return ObjectType::UNIT; }
	void setFormation(short _formation);
	void resetFormation();
	void setPositionInFormation(short _pos);
	void clearAims();
	void removeCurrentAim();
	void setIndexToInteract(int index);
	int getIndexToInteract() const { return indexToInteract; }
	bool closeEnoughToAttack();
	bool isInRightSocket() const;
	void setNextState(UnitState stateTo, const ActionParameter& actionParameter);
	void setNextState(UnitState stateTo);
	ActionParameter& getNextActionParameter();

	static std::string getColumns();
	void changeColor(SimColorMode mode);
	void addOrder(UnitOrder* aim);

	void drawLineTo(const Urho3D::Vector3& second, const Urho3D::Color& color) const;
	void debug(DebugUnitType type, ForceStats& stats);
	bool isFirstThingAlive() const;

	std::tuple<Physical*, float, int> closestPhysical(std::vector<Physical*>* things,
	                                                  const std::function<bool(Physical*)>& condition);

	float getMaxSeparationDistance() const;
	short getPositionInFormation() const { return posInFormation; }
	float getMinimalDistance() const;
	UnitState getActionState() const;
	UnitState getState() const { return state; }
	UnitState getNextState() const { return nextState; }
	short getFormation() const { return formation; }
	bool isToDispose() const override { return state == UnitState::DISPOSE; }
	bool hasAim() const { return aims.hasAim(); }
	db_unit_level* getLevel() const { return dbLevel; }

	float getAttackRange() const;
	bool teamBucketHasChanged(int _bucketIndex, char param) const;
	int getBucketIndex(char param) const { return teamBucketIndex[param]; }
	void setTeamBucket(int _bucketIndex, char param);
	bool isSlotOccupied(int indexToInteract) override;

	Urho3D::Vector2 getPosToUse();
	void setOccupiedSlot(unsigned char index, bool value) override;
	bool ifSlotIsOccupied(unsigned char index) const;

	std::optional<std::tuple<Urho3D::Vector2, float, int>> getPosToUseWithIndex(Unit* follower) override;

	void action(UnitAction unitAction, const ActionParameter& parameter);
	void action(UnitAction unitAction);

	bool isFirstThingInSameSocket() const override;
	std::string getValues(int precision) override;
	Urho3D::String toMultiLineString() override;
	float getMaxHpBarSize() const override;
	bool isAlive() const override;
	char getLevelNum() override;
	short getId() override;
	void clean();
	void fillValues(std::span<float> weights) const;
	void addValues(std::span<float> vals) const override;
	unsigned short getMaxHp() const override;

	Urho3D::Vector2 getSocketPos(Unit* toFollow, int i) const;
private:
	void actionIfCloseEnough(UnitAction order, Physical* closest, float sqDistance, bool force);
	void changeColor(float value, float maxValue) const;
	void setAim(Aim* aim);

	Urho3D::Vector2 velocity, acceleration;
	Aims aims;


	db_unit* dbUnit;
	db_unit_level* dbLevel;

	Urho3D::Material* basic;

	ChargeData* chargeData{};
	MissileData* missileData{};

	std::vector<Physical*> thingsToInteract; //TODO jak to wczytac :O

	int teamBucketIndex[BUCKET_SET_NUMBER];
	int indexToInteract = -1;

	float maxSpeed;

	short posInFormation = -1, formation = -1;
	unsigned short currentFrameState = 0;

	UnitState state;
	UnitState nextState;
	ActionParameter nextActionParameter;
	unsigned char useSockets = 0;
};
