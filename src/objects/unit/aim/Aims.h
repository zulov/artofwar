#pragma once
#include "Aim.h"
#include <Urho3D/Math/Vector2.h>
#include <optional>
#include <vector>


class Entity;
struct FutureAim;

class Aims
{
public:
	Aims();
	~Aims();
	std::optional<Urho3D::Vector2> getDirection(Unit* unit);
	void clearExpired();
	bool ifReach(Unit* unit);
	bool hasAim();
	void add(Aim* aim);
	void add(FutureAim & aim);
	void clear();
private:
	std::vector<FutureAim> nextAims;
	Aim* current;
};
