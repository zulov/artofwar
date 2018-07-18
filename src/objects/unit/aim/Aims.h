#pragma once
#include <Urho3D/Math/Vector2.h>
#include <optional>
#include <vector>
#include <Urho3D/Math/Vector3.h>
#include <vector>


class Aim;
class Unit;
class Entity;
struct FutureAim;

class Aims
{
public:
	Aims();
	~Aims();
	std::optional<Urho3D::Vector2> getDirection(Unit* unit) const;
	void clearExpired();
	bool ifReach(Unit* unit);
	void add(Aim* aim);
	void add(const FutureAim& aim);
	void clear();
	void removeCurrentAim();
	bool hasCurrent() const { return current != nullptr; }
	bool hasAim() const { return hasCurrent() || !nextAims.empty(); }
	std::vector<Urho3D::Vector3> getDebugLines(Urho3D::Vector3* position);
private:
	std::vector<FutureAim> nextAims;
	Aim* current;
};
