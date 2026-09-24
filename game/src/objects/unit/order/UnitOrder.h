#pragma once
#include <vector>

namespace Urho3D {
	class Vector2;
}

class Physical;
class TargetAim;
struct ActionParameter;
enum class UnitAction : unsigned char;
enum class PendingCommandKind : char;
struct UnitOrderSaveData;
struct PendingCommandSaveData;

class UnitOrder {
public:
	UnitOrder(short id, bool append, Physical* toUse);
	UnitOrder(short id, bool append, const Urho3D::Vector2& vector);
	virtual ~UnitOrder();
	virtual void execute();

	virtual bool expired() const = 0;
	virtual bool add() = 0;
	virtual PendingCommandSaveData saveState(unsigned short order) const = 0;
	UnitOrderSaveData saveOrder(unsigned unitUid) const;

	bool getAppend() const { return append; }
	UnitAction getAction() const { return static_cast<UnitAction>(id); }

protected:
	//TODO union or variant
	Physical* toUse = nullptr;
	Urho3D::Vector2* vector = nullptr;

	const unsigned char id;
	const bool append;

	ActionParameter getTargetAim(int startInx, Urho3D::Vector2& to);
	ActionParameter getFollowAim(int startInx, const std::vector<int>& endIdxs);
	ActionParameter getChargeAim(Urho3D::Vector2& charge);
	PendingCommandSaveData createSaveState(unsigned short order, PendingCommandKind kind) const;

private:
	TargetAim* getTargetAimPtr(int startInx, const std::vector<int>& endIdxs) const;
	TargetAim* getTargetAimPtr(int startInx, const Urho3D::Vector2& to) const;
	TargetAim* getAimFromPath(const std::vector<int>* path) const;

	virtual void addCollectAim() = 0;
	virtual void addTargetAim() = 0;
	virtual void addFollowAim() = 0;
	virtual void addChargeAim() = 0;
	virtual void addAttackAim() = 0;
	virtual void addDefendAim() = 0;
	virtual void addDeadAim() = 0;
	virtual void addStopAim() = 0;
};
