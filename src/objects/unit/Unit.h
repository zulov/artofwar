#pragma once

#include <vector>
#include "state/UnitStateType.h"
#include "UnitType.h"
#include "objects/Physical.h"
#include "state/StateManager.h"
#include "objects/resource/ResourceEntity.h"

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
public:
	Unit(Vector3* _position, int id, int player);
	~Unit();
	double getHealthBarSize() override;
	double getHealthPercent() override;
	void populate(db_unit* _dbUnit);
	void checkAim();
	void move(double timeStep);
	void applyForce(double timeStep);
	int getID() override;
	void setAcceleration(Vector3* _acceleration);
	double getMaxSeparationDistance();
	Vector3* getDestination(double boostCoef, double aimCoef);
	Vector3* getVelocity();
	double getUnitRadius();
	double getMinimalDistance();

	void absorbAttack(double attackCoef) override;

	void toAttack(std::vector<Unit*>* enemies);
	void toAttack(Physical* enemy);
	void toAttack();

	void toCollect(std::vector<Physical*>* enemies);
	void toCollect(ResourceEntity* _resource);
	void toCollect();

	void updateHeight(double y, double timeStep);
	String* toMultiLineString() override;
	void action(short id, ActionParameter* parameter) override;
	UnitStateType getState();
	UnitStateType getActionState();
	void clean() override;
	void setState(UnitStateType state);
	bool checkTransition(UnitStateType state);
	void executeState();
	bool hasResource();
	static void setStates(StateManager* _states);
protected:
	Vector3* acceleration;
	Vector3* velocity;
	Aims* aims;
	double mass;
	double maxSpeed;
	double maxSeparationDistance;
	double minSpeed;
	short aimIndex = 0;
private:
	void addAim(ActionParameter* actionParameter);
	void removeAim();
	void attackIfCloseEnough(double& distance, Unit* closest);
	void collectIfCloseEnough(double distance, ResourceEntity* closest);
	ResourceEntity* resource;
	Vector3* toResource;
	db_unit* dbUnit;
	static StateManager* states;

	UnitStateType unitState;
	UnitStateType actionState;
	UnitType unitType;

	double minimalDistance;
	bool rotatable;
	double unitRadius = 2;
	double attackIntrest = 10;
	double collectSpeed = 2;
	static double hbMaxSize;
};
