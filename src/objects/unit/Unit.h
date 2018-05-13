#pragma once

#include "ColorMode.h"
#include "aim/Aims.h"
#include "database/db_strcut.h"
#include "objects/Physical.h"
#include "objects/resource/ResourceEntity.h"
#include "scene/load/dbload_container.h"
#include "state/StateManager.h"
#include "state/UnitStateType.h"
#include <Urho3D/Graphics/StaticModel.h>
#include <vector>


#define MAX_ACCEL 5000

struct FutureAim;
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

	void populate();
	void checkAim();
	void move(double timeStep);
	void applyForce(double timeStep);
	void setAcceleration(Vector2& _acceleration);

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
	void addAim(Aim* aim);


	void setState(UnitStateType _state);
	bool checkTransition(UnitStateType state);
	void executeState();
	bool hasResource();
	void load(dbload_unit* unit);


	void setFormation(short _formation);
	void resetFormation();
	void setPositionInFormation(short _pos);
	void clearAims();

	static std::string getColumns();
	void addUpgrade(db_unit_upgrade* upgrade);
	void changeColor(ColorMode mode);
	void addAim(FutureAim& aim, bool append = false);

	float getMaxSeparationDistance() const { return maxSeparationDistance; }
	UnitStateType getActionState() const { return actionState; }
	short getPositionInFormation() const { return posInFormation; }
	float getMinimalDistance() const { return minimalDistance; }
	UnitStateType getState() const { return state; }
	short getFormation() const { return formation; }
	bool isToDispose() const { return state == UnitStateType::DISPOSE && atState; }
	bool hasAim() { return aims.hasAim(); }

	void action(char id, ActionParameter& parameter) override;
	std::string getValues(int precision) override;
	String& toMultiLineString() override;
	float getMaxHpBarSize() override;
	bool isAlive() const override;
	int getLevel() override;
	int getDbID() override;
	void clean() override;
private:
	//void addAim(Aim* aim);

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

	unsigned short currentFrameState = 0;
};
