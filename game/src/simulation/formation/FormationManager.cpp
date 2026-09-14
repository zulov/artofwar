#include "FormationManager.h"
#include <optional>
#include "objects/unit/Unit.h"
#include "utils/DeleteUtils.h"

FormationManager::~FormationManager() { clear_vector(formations); }

std::optional<Formation*> FormationManager::createFormation(const std::vector<Unit*>& _units, FormationType _type) {
	if (_units.empty()) { return {}; }
	if (_type == FormationType::NONE) {
		for (auto unit : _units) {
			unit->resetFormation();
		}
	} else {
		const short formationInFirst = _units.at(0)->getFormation();
		bool allIn = true;
		if (formationInFirst >= 0
			&& formations[formationInFirst] != nullptr
			&& formations[formationInFirst]->getSize() == _units.size()) {
			for (auto unit : _units) {
				if (unit->getFormation() != formationInFirst) {
					allIn = false;
					break;
				}
			}
			if (allIn) {
				return formations[formationInFirst];
			}
		}

		for (; currentlyFree < formations.size(); ++currentlyFree) {
			if (formations[currentlyFree] == nullptr) {
				break;
			}
		}

		if (currentlyFree == formations.size()) {
			formations.push_back(nullptr);
		}
		auto returnFormation = formations[currentlyFree] = new Formation(currentlyFree, _units, _type, Urho3D::Vector2(1, 1));
		currentlyFree++;
		return returnFormation;
	}
	return {};
}

void FormationManager::update() {
	for (int i = 0; i < formations.size(); ++i) {
		if (formations[i]) {
			formations[i]->update();
			if (formations[i]->getState() == FormationState::EMPTY) {
				delete formations[i];
				formations[i] = nullptr;
				if (i < currentlyFree) {
					currentlyFree = i;
				}
			}
		}
	}
}

float FormationManager::getPriority(Unit* unit) {
	auto formation = getFormation(unit);
	if (formation) {
		return formation->getPriority(unit->getPositionInState());
	}
	return 0.f;
}

bool FormationManager::isLeader(Unit* unit) const {
	auto formation = getFormation(unit);
	return formation && formation->isLeader(unit);
}

bool FormationManager::isMoving(Unit* unit) const {
	auto formation = getFormation(unit);
	return formation && formation->isMoving(unit);
}

Formation* FormationManager::restoreFormation(short id, const std::vector<Unit*>& units, FormationType type,
											  FormationState state, const Urho3D::Vector2& direction) {
	if (id < 0 || units.empty()) { return nullptr; }
	if (formations.size() <= id) {
		formations.resize(id + 1, nullptr);
	}
	delete formations[id];
	formations[id] = new Formation(id, units, type, direction);
	formations[id]->restoreState(state);
	while (currentlyFree < formations.size() && formations[currentlyFree]) {
		++currentlyFree;
	}
	return formations[id];
}
