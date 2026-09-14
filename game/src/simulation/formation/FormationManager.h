#pragma once
#include <span>
#include <vector>
#include "Formation.h"

#include "objects/unit/Unit.h"

class FormationManager {
public:
	FormationManager() = default;
	~FormationManager();
	std::optional<Formation*> createFormation(const std::vector<Unit*>& _units, FormationType _type = FormationType::SQUERE);
	void update();
	float getPriority(Unit* unit);
	bool isLeader(Unit* unit) const;
	bool isMoving(Unit* unit) const;
	Formation* getFormation(Unit* unit) const { return getFormation(unit->getFormation()); }
	Formation* getFormation(short id) const { return id >= 0 && id < formations.size() ? formations[id] : nullptr; }
	std::span<Formation* const> getFormations() const { return formations; }
	Formation* restoreFormation(short id, const std::vector<Unit*>& units, FormationType type, FormationState state,
							const Urho3D::Vector2& direction);

private:
	std::vector<Formation*> formations;
	unsigned currentlyFree = 0;
};
