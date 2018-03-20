#pragma once
#include "Aim.h"
#include <Urho3D/Math/Vector2.h>
#include <optional>
#include <vector>


class Entity;

class Aims
{
public:
	Aims();
	~Aims();
	std::optional<Urho3D::Vector2> getDirection(Unit* unit);
	void clearAimsIfExpired();
	bool ifReach(Unit* unit);
	bool hasAim();
	void add(Aim* aim);
	void clear();
private:
	std::vector<Aim*> aims;
	int current;
};
