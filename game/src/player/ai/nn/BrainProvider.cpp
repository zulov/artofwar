#include "BrainProvider.h"

#include <Urho3D/IO/Log.h>

#include "Brain.h"
#include "Game.h"
#include "utils/DeleteUtils.h"
#include "utils/FileUtils.h"

std::vector<Brain*> BrainProvider::brains;
std::vector<LayerData> BrainProvider::tempLayers;

BrainProvider::~BrainProvider() {
	clear_vector(brains);
}

Brain* BrainProvider::get(const std::string name) {
	for (const auto brain : brains) {
		if (brain->getName() == name) {
			return brain;
		}
	}
	if (!loadBrainFile(name, tempLayers) || tempLayers.empty()) {
		assert(false);
		Game::getLog()->WriteRaw("No brain Found " + Urho3D::String(name.c_str()) + "\n", true);
		return nullptr;
	}
	auto* const brain = new Brain(name, tempLayers);
	brains.push_back(brain);
	return brain;
}
