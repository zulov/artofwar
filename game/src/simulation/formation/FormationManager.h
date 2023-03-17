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
	bool isMoving(Unit* unit) const;
	int getCachePath(Unit* unit, int aimIndex) const;
	void addCachePath(Unit* unit, int aimIndex, int next);
private:
	std::vector<Formation*> formations;
	unsigned currentlyFree = 0;
};
