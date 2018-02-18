#pragma once

#include "objects/Physical.h"
#include "objects/resource/ResourceEntity.h"
#include "scene/load/dbload_container.h"
#include "state/StateManager.h"
#include "state/UnitStateType.h"
#include <vector>


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
	Unit(Vector3* _position, int id, int player);
	~Unit();

	bool isAlive() override;
	float getHealthBarSize() override;
	float getHealthPercent() override;
	void populate(db_unit* _dbUnit);
	void checkAim();
	void move(double timeStep);
	void applyForce(double timeStep);
	int getDbID() override;
	void setAcceleration(Vector3* _acceleration);
	float getMaxSeparationDistance();
	Vector3* getDestination(double boostCoef, double aimCoef);

	float getMinimalDistance();

	void absorbAttack(double attackCoef) override;

	void toAttack(std::vector<Unit*>* enemies);
	void toAttack(Physical* enemy);
	void toAttack();

	void toCollect(std::vector<Physical*>* enemies);
	void toCollect(ResourceEntity* _resource);
	void toCollect();

	void updateHeight(double y, double timeStep);
	String& toMultiLineString() override;
	void action(short id, ActionParameter& parameter) override;
	std::string getValues(int precision) override;

	UnitStateType getState();
	UnitStateType getActionState();
	void clean() override;
	void setState(UnitStateType _state);
	bool checkTransition(UnitStateType state);
	void executeState();
	bool hasResource();
	void load(dbload_unit* unit);
	bool isToDispose();

	static std::string getColumns();
private:
	void addAim(Aim* aim, bool append);
	void removeAim();
	void attackIfCloseEnough(double& distance, Unit* closest);
	void collectIfCloseEnough(double distance, ResourceEntity* closest);

	Vector3 acceleration;
	Vector3* velocity;
	Aims aims;

	ResourceEntity* resource;
	Vector3* toResource;
	db_unit* dbUnit;

	UnitStateType state;
	UnitStateType actionState;

	bool rotatable;
	float minimalDistance;
	float attackIntrest = 10;
	float collectSpeed = 2;
	float mass;
	float maxSpeed;
	float minSpeed;
	float maxSeparationDistance;

	static float hbMaxSize;

	bool atState = false;
};
