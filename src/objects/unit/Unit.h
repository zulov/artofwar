#pragma once

#include "aim/Aims.h"
#include "objects/Physical.h"
#include "state/UnitState.h"
#include <Urho3D/Graphics/CustomGeometry.h>
#include <vector>


#define BUCKET_SET_NUMBER 4
#define USE_SOCKETS_NUMBER 8

enum class DebugUnitType : char;
enum class ColorMode : char;
struct ForceStats;
struct db_unit_upgrade;
struct db_unit_level;
struct db_unit;
struct dbload_unit;
class FutureOrder;
struct MissileData;
struct ChargeData;

class Aim;
class State;
class ResourceEntity;

class Unit : public Physical
{
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
public:
	Unit(Urho3D::Vector3* _position, int id, int player, int level);
	~Unit();

	void populate() override;
	void checkAim();
	void move(double timeStep) const;
	void applyForce(double timeStep);
	void setAcceleration(Urho3D::Vector2& _acceleration);

	void forceGo(float boostCoef, float aimCoef, Urho3D::Vector2& force) const;
	Urho3D::Vector2 getDestination(float boostCoef, float aimCoef);

	void absorbAttack(float attackCoef) override;

	void toCharge(std::vector<Physical*>* enemies);

	void toAction(Physical* closest, float minDistance, int indexToInteract, UnitState stateTo);
	void toAction(Physical* closest, float minDistance, int indexToInteract, UnitState stateTo, float attackIntrest);

	void interactWithOne(Physical* thing, int indexToInteract, UnitState action);

	void updateHeight(float y, double timeStep);

	void setState(UnitState _state);

	bool hasResource();
	void load(dbload_unit* unit);

	void setFormation(short _formation);
	void resetFormation();
	void setPositionInFormation(short _pos);
	void clearAims();
	void removeCurrentAim();
	void setIndexToInteract(int index);
	int getIndexToInteract() const { return indexToInteract; }
	bool closeEnoughToAttack() const;

	static std::string getColumns();
	void addUpgrade(db_unit_upgrade* upgrade);
	void changeColor(ColorMode mode);
	void addAim(FutureOrder* aim, bool append = false);
	void drawLineTo(const Urho3D::Vector3& second, const Urho3D::Color& color) const;
	void debug(DebugUnitType type, ForceStats& stats);
	bool isFirstThingAlive() const;
	bool hasEnemy();

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

	bool bucketHasChanged(int _bucketIndex, char param) const;
	int getBucketIndex(char param) const { return teamBucketIndex[param]; }
	void setBucket(int _bucketIndex, char param);
	bool isSlotOccupied(int indexToInteract) override { return useSockets[indexToInteract]; }
	void setOccupiedSlot(int indexToInteract, bool value) { useSockets[indexToInteract] = value; }
	Urho3D::Vector2 getPosToUse() const;

	std::optional<std::tuple<Urho3D::Vector2, float, int>> getPosToUseWithIndex(Unit* unit) const override;
	void action(char id, const ActionParameter& parameter) override;
	std::string getValues(int precision) override;
	Urho3D::String& toMultiLineString() override;
	float getMaxHpBarSize() override;
	bool isAlive() const override;
	int getLevel() override;
	int getDbID() override;
	void clean() override;
	Urho3D::Vector2 getSocketPos(const Unit* unit, int i) const;
private:
	void actionIfCloseEnough(UnitState action, Physical* closest, int indexToInteract,
	                         float sqDistance, float closeRange, float intrestRange);
	void changeColor(float value, float maxValue) const;

	Urho3D::Vector2 velocity, acceleration;
	Aims aims;

	db_unit* dbUnit;
	db_unit_level* dbLevel;

	UnitState state, actionState;

	Urho3D::Material* basic;

	ChargeData* chargeData{};
	MissileData* missileData{};

	std::vector<db_unit_upgrade*> upgrades;

	bool atState = false, rotatable;
	float minimalDistance, maxSeparationDistance,
	      maxSpeed, minSpeed, mass,
	      attackInterest, collectSpeed;

	short posInFormation = -1, formation = -1;

	int teamBucketIndex[BUCKET_SET_NUMBER];

	int indexToInteract = -1;
	unsigned short currentFrameState = 0;

	std::vector<Physical*> thingsToInteract; //TODO jak to wczytac :O

	bool useSockets[USE_SOCKETS_NUMBER];
};
