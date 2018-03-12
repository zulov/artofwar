#include "FormationManager.h"
#include "objects/unit/Unit.h"
#include "utils.h"
#include <optional>


FormationManager::FormationManager() = default;


FormationManager::~FormationManager() {
	clear_vector(formations);
}

void FormationManager::createFormation(std::vector<Physical*>* _units, FormationType _type) {
	for (; currentlyFree < formations.size(); ++currentlyFree) {
		if (formations[currentlyFree] == nullptr) {
			break;
		}
	}

	if (currentlyFree == formations.size()) {
		formations.push_back(nullptr);
	}
	formations[currentlyFree] = new Formation(currentlyFree, _units, _type);
	currentlyFree++;
}

void FormationManager::update() {
	for (int i = 0; i < formations.size(); ++i) {
		if (formations[i]) {
			bool stilExists = formations[i]->update();
			if (!stilExists) {
				delete formations[i];
				formations[i] = nullptr;
				if (i < currentlyFree) {
					currentlyFree = i;
				}
			}
		}
	}
}

std::optional<Vector3> FormationManager::getPositionFor(Unit* unit) {
	const short formation = unit->getFormation();
	if (formation >= 0) {
		return formations[formation]->getPositionFor(unit->getPositionInFormation());
	}
	return std::nullopt;
}
