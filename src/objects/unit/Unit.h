#pragma once

#include <Urho3D/Container/Str.h>
#include <Urho3D/Math/Vector2.h>
#include <tuple>

#include "ActionParameter.h"
#include "aim/Aims.h"
#include "objects/ObjectEnums.h"
#include "objects/Physical.h"
#include "state/UnitState.h"

class IndividualOrder;

namespace Urho3D {
	class Vector4;
	class Color;
	class Material;
	class Vector3;
}

constexpr char USE_SOCKETS_NUMBER = 8;

enum class UnitAction : char;
enum class DebugUnitType : char;
enum class SimColorMode : char;
struct ForceStats;
struct db_unit_level;
struct db_unit;
struct dbload_unit;
struct ProjectileWithNode;
struct ChargeData;
struct ActionParameter;
struct CameraInfo;

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
	~Unit() override;

	void populate() override;
	void checkAim();
	void updatePosition() const;
	bool move(float timeStep, const CameraInfo* camInfo);
	bool ifVisible(bool hasMoved, const CameraInfo* camInfo) const;
	void applyForce(float timeStep);
	void setAcceleration(Urho3D::Vector2& _acceleration);

	void forceGo(float boostCoef, float aimCoef, Urho3D::Vector2& force) const;
	Urho3D::Vector2 getDestination(float boostCoef, float aimCoef);

	std::pair<float, bool> absorbAttack(float attackCoef) override;

	bool toActionIfInRange(Physical* closest, UnitAction order);
	bool toAction(Physical* closest, UnitAction order);

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

	void setNextState(UnitState stateTo, const ActionParameter& actionParameter);
	void setNextState(UnitState stateTo);
	ActionParameter& getNextActionParameter();
	bool hasStateChangePending() const;

	void changeColor(SimColorMode mode);
	void addOrder(IndividualOrder* aim);

	void drawLineTo(const Urho3D::Vector3& second, const Urho3D::Color& color) const;
	void debug(DebugUnitType type, ForceStats& stats);
	bool isFirstThingAlive() const;

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
	short getLastActionThingId() const { return lastActionThingId; }
	float getAttackRange() const;

	int getSparseIndex() const { return sparseIndexInGrid; }

	bool isIndexSlotOccupied(int indexToInteract) override;

	void setOccupiedIndexSlot(char index, bool value) override;
	bool ifSlotIsOccupied(char index) const;

	std::optional<std::tuple<Urho3D::Vector2, float>> getPosToUseWithDist(Unit* user) override;
	std::vector<int> getIndexesForUse(Unit* user) const override;
	std::vector<int> getIndexesForRangeUse(Unit* user) const override;

	bool action(UnitAction unitAction, const ActionParameter& parameter);
	bool action(UnitAction unitAction);

	bool indexChanged() const override { return indexHasChanged; }
	std::string getValues(int precision) override;
	Urho3D::String getInfo() const override;
	const Urho3D::String& getName() const override;

	float getMaxHpBarSize() const override;
	bool isAlive() const override;
	char getLevelNum() override;
	void clean();
	void fillValues(std::span<float> weights) const;
	void addValues(std::span<float> vals) const override;
	float getSightRadius() const override;
	Urho3D::Vector2 getSocketPos(Unit* toFollow, int i) const;
	short getCostSum() const override;
	bool isInCloseRange(int index) const override;
	void setSlotToInteract(char slot) { slotToInteract = slot; }
	void setSparseIndex(int index);

	float getModelHeight() const override;
	void setModelData(float modelHeight) const override;

	void setThingToInteract(Physical* toUse) {
		thingToInteract = toUse;
	}

	unsigned char getMaxRangeUsers() const { return 16; }

private:
	void changeColor(float value, float maxValue) const;
	void setAim(Aim* aim);
	Urho3D::StaticModel* model{};
	Urho3D::Vector2 velocity, acceleration;
	Aims aims;

	db_unit* dbUnit;
	db_unit_level* dbLevel;

	Urho3D::Material* basic{};

	ChargeData* chargeData{};

	Physical* thingToInteract{}; //TODO jak to wczytac :O

	int sparseIndexInGrid;

	float maxSpeed;

	int indexToInteract = -1; //TODO moze sie tego pozbyc

	unsigned short currentFrameState = 0;
	short posInFormation = -1, formation = -1;

	UnitState state;
	UnitState nextState;
	bool stateChangePending = false;
	char slotToInteract = -1;
	unsigned char useSockets = 0;
	char lastActionThingId = -1; //TODO reset po jakimœ czasie
	ActionParameter nextActionParameter;
};
