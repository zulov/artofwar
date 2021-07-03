#include "BrainProvider.h"
#include "Brain.h"
#include "utils/DeleteUtils.h"

std::vector<Brain*> BrainProvider::brains;

BrainProvider::~BrainProvider() {
	clear_vector(brains);
}

Brain* BrainProvider::get(const std::string name) {
	for (auto brain : brains) {
		if (brain->getName() == name) {
			return brain;
		}
	}
	auto* const brain = new Brain(name);
	brains.push_back(brain);
	return brain;
}
