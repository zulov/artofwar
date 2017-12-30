#pragma once

#include <vector>
#include "state/UnitStateType.h"
#include "objects/Physical.h"
#include "state/StateManager.h"
#include "objects/resource/ResourceEntity.h"
#include "scene/load/dbload_container.h"

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
	Vector3* getVelocity();
	float getUnitRadius();
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
	void action(short id, ActionParameter* parameter) override;
	std::string getValues(int precision) override;

	UnitStateType getState();
	UnitStateType getActionState();
	void clean() override;
	void setState(UnitStateType state);
	bool checkTransition(UnitStateType state);
	void executeState();
	bool hasResource();
	void load(dbload_unit* unit);

	static std::string getColumns();
protected:
	Vector3* acceleration;
	Vector3* velocity;
	Aims* aims;
	float mass;
	float maxSpeed;
	float maxSeparationDistance;
	float minSpeed;
	short aimIndex = 0;
private:
	void addAim(ActionParameter* actionParameter);
	void removeAim();
	void attackIfCloseEnough(double& distance, Unit* closest);
	void collectIfCloseEnough(double distance, ResourceEntity* closest);
	ResourceEntity* resource;
	Vector3* toResource;
	db_unit* dbUnit;

	UnitStateType unitState;
	UnitStateType actionState;

	float minimalDistance;
	bool rotatable;
	float unitRadius = 2;
	float attackIntrest = 10;
	float collectSpeed = 2;
	static float hbMaxSize;
};
