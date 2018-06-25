#pragma once

#include "objects/Physical.h"
#include "state/UnitState.h"
#include "aim/Aims.h"
#include <Urho3D/Graphics/StaticModel.h>
#include <vector>
#include <tuple>

enum class ColorMode : char;
struct db_unit_upgrade;
struct db_unit_level;
struct db_unit;
struct dbload_unit;
struct FutureAim;
struct MissleData;

class Aim;
class State;
class ResourceEntity;
class ChargeData;

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
	Unit(Vector3* _position, int id, int player, int level);
	~Unit();

	void populate();
	void checkAim();
	void move(double timeStep);
	void applyForce(double timeStep);
	void setAcceleration(Vector2& _acceleration);

	Vector2 forceGo(float boostCoef, float aimCoef, Vector2& force) const;
	Vector2 getDestination(float boostCoef, float aimCoef);

	void absorbAttack(float attackCoef) override;

	void toAttack(std::vector<Physical*>* enemies);
	void toCollect(std::vector<Physical*>* entities);
	void toShot(std::vector<Physical*>* enemies);

	void toCharge(std::vector<Physical*>* enemies);

	void oneToInteract(Physical* enemy, UnitState action);

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

	static std::string getColumns();
	void addUpgrade(db_unit_upgrade* upgrade);
	void changeColor(ColorMode mode);
	void addAim(const FutureAim& aim, bool append = false);

	float getMaxSeparationDistance() const { return maxSeparationDistance; }
	UnitState getActionState() const { return actionState; }
	short getPositionInFormation() const { return posInFormation; }
	float getMinimalDistance() const { return minimalDistance; }
	UnitState getState() const { return state; }
	short getFormation() const { return formation; }
	bool isToDispose() const { return state == UnitState::DISPOSE && atState; }
	bool hasAim() const { return aims.hasAim(); }

	void action(char id, ActionParameter& parameter) override;
	std::string getValues(int precision) override;
	String& toMultiLineString() override;
	float getMaxHpBarSize() override;
	bool isAlive() const override;
	int getLevel() override;
	int getDbID() override;
	void clean() override;
private:

	void attackIfCloseEnough(float distance, Physical* closest);
	void collectIfCloseEnough(float distance, Physical* closest);
	void shotIfCloseEnough(float distance, Physical* closest);

	void changeColor(float value, float maxValue) const;
	void changeColor(Material* newMaterial) const;

	Vector2 acceleration;
	Vector2 velocity;
	Aims aims;

	db_unit* dbUnit;
	db_unit_level* dbLevel;

	UnitState state;
	UnitState actionState;

	StaticModel* model;
	Material* basic;

	ChargeData* chargeData{};
	MissleData* missleData{};

	std::vector<db_unit_upgrade*> upgrades;

	bool rotatable;
	bool atState = false;
	float minimalDistance;
	float attackIntrest = 10;
	float collectSpeed = 2;
	float mass;
	float maxSpeed;
	float minSpeed;
	float maxSeparationDistance;

	short posInFormation = -1;
	short formation = -1;

	unsigned short currentFrameState = 0;
};
