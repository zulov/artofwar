#pragma once
#include <Urho3D/Math/Vector2.h>
#include <Urho3D/Math/Vector3.h>
#include <optional>
#include <vector>


class Aim;
class Unit;
class Entity;
class FutureOrder;

class Aims
{
public:
	Aims();
	~Aims();
	std::optional<Urho3D::Vector2> getDirection(Unit* unit) const;
	void clearExpired();
	bool ifReach(Unit* unit);
	void add(Unit* unit, FutureOrder& aim, bool append);
	void clear();
	void removeCurrentAim();
	bool hasCurrent() const { return current != nullptr; }
	bool hasAim() const { return hasCurrent() || !nextAims.empty(); }
	std::vector<Urho3D::Vector3> getDebugLines(Unit* unit) const;
private:
	void next();
	std::vector<FutureOrder> nextAims;
	Aim* current;
};
