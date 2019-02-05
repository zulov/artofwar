#pragma once
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
	void add(FutureOrder* aim);
	void clear();
	void removeCurrentAim();
	bool hasCurrent() const { return current != nullptr; }
	bool hasAim() const { return hasCurrent() || !nextAims.empty(); }
	std::vector<Urho3D::Vector3> getDebugLines(Unit* unit) const;
	void set(Aim* aim);
private:
	std::vector<FutureOrder*> nextAims;
	Aim* current;
};
