#include "BrainProvider.h"

#include <Urho3D/IO/Log.h>

#include "Brain.h"
#include "Game.h"
#include "utils/DeleteUtils.h"
#include "utils/FileUtils.h"

std::vector<Brain*> BrainProvider::brains;
std::vector<LayerData> BrainProvider::tempLayers;
std::string BrainProvider::fileBuffer;

BrainProvider::~BrainProvider() {
	clear_vector(brains);
}

Brain* BrainProvider::get(const std::string name) {
	for (const auto brain : brains) {
		if (brain->getName() == name) {
			return brain;
		}
	}
	if (!loadBrainFile(name, tempLayers, fileBuffer) || tempLayers.empty()) {
		// Always-on diagnostic: a missing or unparseable brain file is fatal to the
		// AI, so report it in Release builds too (not just behind assert).
		Game::getLog()->WriteRaw("[Brain] FAILED to load brain file '"
			+ Urho3D::String(name.c_str()) + "' (missing or empty)\n", true);
		assert(false);
		return nullptr;
	}
	auto* const brain = new Brain(name, tempLayers);
	brains.push_back(brain);
	return brain;
}
