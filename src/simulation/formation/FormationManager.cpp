#include "FormationManager.h"
#include "utils.h"


FormationManager::FormationManager() {
}


FormationManager::~FormationManager() {
	clear_vector(formations);
}

void FormationManager::createFormation(std::vector<Physical*>* _units, FormationType _type) {
	formations.push_back(new Formation(formations.size(), _units, _type));
}

void FormationManager::update() {
	for (auto formation : formations) {
		formation->update();
	}
}
