#pragma once

#include "ColorMode.h"
#include "objects/Physical.h"
#include "objects/resource/ResourceEntity.h"
#include "scene/load/dbload_container.h"
#include "state/StateManager.h"
#include "state/UnitStateType.h"
#include <Urho3D/Graphics/StaticModel.h>
#include <vector>


#define MAX_ACCEL 5000

class State;

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
public:
	Unit(Vector3* _position, int id, int player, int level);
	~Unit();

	bool isAlive() override;

	void populate();
	void checkAim();
	void move(double timeStep);
	void applyForce(double timeStep);
	void setAcceleration(Vector2& _acceleration);
	int getDbID() override;
	int getLevel() override;
	float getMaxHpBarSize() override;
	float getMaxSeparationDistance() const { return maxSeparationDistance; }
	float getMinimalDistance() const { return minimalDistance; }
	Vector2 forceGo(float boostCoef, float aimCoef, Vector2& force);
	Vector2 getDestination(float boostCoef, float aimCoef);


	void absorbAttack(float attackCoef) override;

	void toAttack(std::vector<Unit*>* enemies);
	void toAttack(Physical* enemy);
	void toAttack();

	void toCollect(std::vector<Physical*>* enemies);
	void toCollect(ResourceEntity* _resource);
	void toCollect();

	void updateHeight(float y, double timeStep);
	String& toMultiLineString() override;
	void action(short id, ActionParameter& parameter) override;

	UnitStateType getState() { return state; }
	UnitStateType getActionState();
	void clean() override;
	void setState(UnitStateType _state);
	bool checkTransition(UnitStateType state);
	void executeState();
	bool hasResource();
	void load(dbload_unit* unit);
	bool isToDispose();

	short getFormation();
	short getPositionInFormation();
	void setFormation(short _formation);
	void setPositionInFormation(short _pos);

	static std::string getColumns();
	std::string getValues(int precision) override;
	void addUpgrade(db_unit_upgrade* upgrade);
	void changeColor(ColorMode mode);
private:
	void addAim(Aim* aim, bool append);
	void removeAim();
	void attackIfCloseEnough(float& distance, Unit* closest);
	void collectIfCloseEnough(float distance, ResourceEntity* closest);

	void changeColor(float value, float maxValue);
	void changeColor(Material* newMaterial);
	void changeColor(UnitStateType state);

	Vector2 acceleration;
	Vector2 velocity;
	Aims aims;

	ResourceEntity* resource;
	Vector2* toResource;
	db_unit* dbUnit;
	db_unit_level* dbLevel;

	UnitStateType state;
	UnitStateType actionState;

	StaticModel* model;
	Material* basic;

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
};
