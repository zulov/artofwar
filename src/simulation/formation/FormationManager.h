#pragma once
#include "Formation.h"
#include <optional>
#include <vector>


class FormationManager {
public:
	FormationManager() = default;
	~FormationManager();
	std::optional<Formation*> createFormation(const std::vector<Unit*>& _units,
	                                          FormationType _type = FormationType::SQUERE);
	void update();
	float getPriority(Unit* unit);
	std::optional<Urho3D::Vector2> getPositionFor(Unit* unit);
	bool isLeader(Unit* unit) const;
private:
	std::vector<Formation*> formations;
	unsigned currentlyFree = 0;
};
