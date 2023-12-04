#pragma once
#include <optional>
#include <vector>

namespace Urho3D {
	class Vector3;
	class Vector2;
}

class IndividualOrder;
class Aim;
class Unit;

class Aims {
public:
	Aims();
	Aims(const Aims& rhs) = delete;
	~Aims();
	Urho3D::Vector2 getDirection(Unit* unit) const;
	void clearExpired();
	bool process(Unit* unit);
	void add(IndividualOrder* order);
	void clear();
	void removeCurrentAim();
	bool hasCurrent() const { return current != nullptr; }
	bool hasAim() const { return hasCurrent() || !nextAims.empty(); }
	std::vector<Urho3D::Vector3> getDebugLines(Unit* unit) const;
	void set(Aim* aim);
private:
	std::vector<IndividualOrder*> nextAims;
	Aim* current;
};
