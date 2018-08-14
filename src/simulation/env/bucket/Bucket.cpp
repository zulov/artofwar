#include "Bucket.h"
#include "objects/Physical.h"
#include "simulation/env/ContentInfo.h"


Bucket::Bucket() {
	std::fill_n(unitsNumberPerPlayer, MAX_PLAYERS, 0);
}

Bucket::~Bucket() = default;

void Bucket::add(Physical* entity) {
	content.push_back(entity);
	++unitsNumberPerPlayer[entity->getPlayer()];
}

void Bucket::remove(Physical* entity) {
	ptrdiff_t pos = std::find(content.begin(), content.end(), entity) - content.begin();
	//	if (pos < content->size()) {
	//		std::iter_swap(content->begin() + pos, content->end()-1);
	//		content->erase(content->end()-1);
	//		--size;
	//	}

	if (pos < content.size()) {
		content.erase(content.begin() + pos);
		--unitsNumberPerPlayer[entity->getPlayer()];
	}

}

bool Bucket::incUnitsPerPlayer(content_info* ci, int activePlayer, const bool checks[]) {
	bool hasUnits = false;
	if (!content.empty()) {
		for (int i = 0; i < MAX_PLAYERS; ++i) {
			if ((checks[3] && i == activePlayer || checks[4] && i != activePlayer) && unitsNumberPerPlayer[i] > 0) {
				ci->unitsNumberPerPlayer[i] += unitsNumberPerPlayer[i];
				hasUnits = true;
			}
		}
		return hasUnits;
	}
	return false;
}
