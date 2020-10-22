#pragma once
#include <optional>
#include <vector>
#include <Urho3D/Math/Vector2.h>
#include <Urho3D/Math/Vector3.h>


class UnitOrder;
class Aim;
class Unit;

class Aims {
public:
	Aims();
	Aims(const Aims& rhs) = delete;
	~Aims();
	std::optional<Urho3D::Vector2> getDirection(Unit* unit) const;
	void clearExpired();
	bool ifReach(Unit* unit);
	void add(UnitOrder* order);
	void clear();
	void removeCurrentAim();
	bool hasCurrent() const { return current != nullptr; }
	bool hasAim() const { return hasCurrent() || !nextAims.empty(); }
	std::vector<Urho3D::Vector3> getDebugLines(Unit* unit) const;
	void set(Aim* aim);
private:
	std::vector<UnitOrder*> nextAims;
	Aim* current;
};
