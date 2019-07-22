#include "InfluenceManager.h"
#include "objects/unit/Unit.h"
#include "simulation/env/EnvConsts.h"
#include "simulation/env/ContentInfo.h"

InfluenceManager::InfluenceManager(char numberOfPlayers) {
	for (int i = 0; i < numberOfPlayers; ++i) {
		unitsNumberPerPlayer.emplace_back(new InfluenceMapInt(DEFAULT_INF_GRID_SIZE,BUCKET_GRID_SIZE));
	}
	ci = new content_info();
}

InfluenceManager::~InfluenceManager() {
	for (auto map : unitsNumberPerPlayer) {
		delete map;
	}
	delete ci;
}

void InfluenceManager::update(std::vector<Unit*>* units) const {
	for (auto map : unitsNumberPerPlayer) {
		map->reset();
	}
	for (auto unit : (*units)) {
		unitsNumberPerPlayer[unit->getPlayer()]->update(unit);
	}
}


content_info* InfluenceManager::getContentInfo(const Urho3D::Vector2& from, const Urho3D::Vector2& to, bool checks[],
                                               int activePlayer) {
	const auto posBeginX = calculator.getIndex(from.x_);
	const auto posBeginZ = calculator.getIndex(from.y_);
	const auto posEndX = calculator.getIndex(to.x_);
	const auto posEndZ = calculator.getIndex(to.y_);

	const auto iMin = Urho3D::Min(posBeginX, posEndX);
	const auto iMax = Urho3D::Max(posBeginX, posEndX);
	const auto jMin = Urho3D::Min(posBeginZ, posEndZ);
	const auto jMax = Urho3D::Max(posBeginZ, posEndZ);

	ci->reset();

	for (short i = iMin; i < iMax; ++i) {
		for (short j = jMin; j < jMax; ++j) {
			const int index = i * resolution + j;
			updateInfo(index, ci, checks, activePlayer);
		}
	}
	return ci;
}
