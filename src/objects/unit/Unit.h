#pragma once

#include "aim/Aims.h"
#include "objects/Physical.h"
#include "state/UnitState.h"
#include <Urho3D/Graphics/CustomGeometry.h>
#include <vector>


#define BUCKET_SET_NUMBER 4

enum class DebugUnitType : char;
enum class ColorMode : char;
struct ForceStats;
struct db_unit_upgrade;
struct db_unit_level;
struct db_unit;
struct dbload_unit;
struct FutureAim;
struct MissleData;
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
	void addAim(Aim* aim);

	void setState(UnitState _state);

	bool hasResource();
	void load(dbload_unit* unit);

	void setFormation(short _formation);
	void resetFormation();
	void setPositionInFormation(short _pos);
	void clearAims();
	void removeCurrentAim();
	void setIndexToInteract(int index);
	int getIndexToInteract() { return indexToInteract; }

	static std::string getColumns();
	void addUpgrade(db_unit_upgrade* upgrade);
	void changeColor(ColorMode mode);
	void addAim(const FutureAim& aim, bool append = false);
	void drawLineTo(const Urho3D::Vector3& second, const Urho3D::Color& color) const;
	void drawLine(const Urho3D::Vector3& first, const Urho3D::Vector3& second,
	              const Urho3D::Color& color) const;
	void debug(DebugUnitType type, ForceStats& stats);

	float getMaxSeparationDistance() const { return maxSeparationDistance; }
	UnitState getActionState() const { return actionState; }
	short getPositionInFormation() const { return posInFormation; }
	float getMinimalDistance() const { return minimalDistance; }
	UnitState getState() const { return state; }
	short getFormation() const { return formation; }
	bool isToDispose() const override { return state == UnitState::DISPOSE && atState; }
	bool hasAim() const { return aims.hasAim(); }

	bool bucketHasChanged(int _bucketIndex, char param) const;
	int getBucketIndex(char param) const { return teamBucketIndex[param]; }
	void setBucket(int _bucketIndex, char param);
	void action(char id, const ActionParameter& parameter) override;
	std::string getValues(int precision) override;
	Urho3D::String& toMultiLineString() override;
	float getMaxHpBarSize() override;
	bool isAlive() const override;
	int getLevel() override;
	int getDbID() override;
	void clean() override;
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
	MissleData* missleData{};

	std::vector<db_unit_upgrade*> upgrades;

	bool rotatable;
	bool atState = false;
	float minimalDistance, maxSeparationDistance;
	float maxSpeed, minSpeed;
	float attackInterest = 10;
	float collectSpeed = 2;
	float mass;

	short posInFormation = -1;
	short formation = -1;

	int teamBucketIndex[BUCKET_SET_NUMBER];

	int indexToInteract = -1;

	unsigned short currentFrameState = 0;

};
