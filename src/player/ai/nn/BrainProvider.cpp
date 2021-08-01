#include "BrainProvider.h"

#include <Urho3D/IO/Log.h>

#include "Brain.h"
#include "Game.h"
#include "utils/DeleteUtils.h"
#include "utils/FileUtils.h"

std::vector<Brain*> BrainProvider::brains;
std::vector<std::string> BrainProvider::tempLines;

BrainProvider::~BrainProvider() {
	clear_vector(brains);
}

Brain* BrainProvider::get(const std::string name) {
	for (auto brain : brains) {
		if (brain->getName() == name) {
			return brain;
		}
	}
	loadLines(name, tempLines);
	assert(!tempLines.empty());
	if (tempLines.empty()) {
		Game::getLog()->WriteRaw("No brain Found " + Urho3D::String(name.c_str()) + "\n", true);
		return nullptr;
	}
	auto* const brain = new Brain(name, tempLines);
	brains.push_back(brain);
	return brain;
}
