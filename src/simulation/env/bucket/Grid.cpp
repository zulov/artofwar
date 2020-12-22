#include "Grid.h"
#include "Bucket.h"
#include "levels/LevelCache.h"
#include "levels/LevelCacheProvider.h"
#include "objects/unit/Unit.h"
#include "simulation/env/CloseIndexes.h"
#include "simulation/env/CloseIndexesProvider.h"
#include "simulation/env/ContentInfo.h"
#include "simulation/env/GridCalculator.h"
#include "simulation/env/GridCalculatorProvider.h"
#include "utils/consts.h"

Grid::Grid(short resolution, float size, float maxQueryRadius)
	: calculator(GridCalculatorProvider::get(resolution, size)),
	  levelCache(LevelCacheProvider::get(resolution, maxQueryRadius, calculator)),
	  closeIndexes(CloseIndexesProvider::get(resolution)),
	  resolution(resolution), sqResolution(resolution * resolution) {
	buckets = new Bucket[sqResolution];
	tempSelected = new std::vector<Physical*>();
}

Grid::~Grid() {
	delete[] buckets;
	delete tempSelected;
}

void Grid::update(Unit* unit, const char team) const {
	assert(!unit->isToDispose());

	const int index = calculator->indexFromPosition(unit->getPosition());
	if (unit->teamBucketHasChanged(index, team)) {
		removeAt(unit->getBucketIndex(team), unit);
		addAt(index, unit);
		unit->setTeamBucket(index, team);
	}
}

void Grid::update(Physical* entity) const {
	assert(!entity->isToDispose());

	const int index = calculator->indexFromPosition(entity->getPosition());
	if (entity->bucketHasChanged(index)) {
		removeAt(entity->getMainBucketIndex(), entity);
		addAt(index, entity);
		entity->setBucket(index);
	} else {
		entity->indexHasChangedReset();
	}
}

void Grid::remove(Unit* unit, char team) const {
	removeAt(unit->getBucketIndex(team), unit);
}

void Grid::remove(Physical* entity) const {
	removeAt(entity->getMainBucketIndex(), entity);
}

void Grid::updateNew(Physical* physical) const {
	assert(physical->getMainBucketIndex(), -1);
	const int index = calculator->indexFromPosition(physical->getPosition());
	addAt(index, physical);
	physical->setBucket(index);
}

void Grid::updateNew(Unit* unit, char team) const {
	assert(unit->getBucketIndex(team), -1);
	const int index = calculator->indexFromPosition(unit->getPosition());
	addAt(index, unit);
	unit->setTeamBucket(index, team);
}


BucketIterator& Grid::getArrayNeight(Urho3D::Vector3& position, float radius) {
	return *iterator.init(levelCache->get(radius), calculator->indexFromPosition(position), this);
}

BucketIterator& Grid::getArrayNeight(int center, float radius) {
	return *iterator.init(levelCache->get(radius), center, this);
}

const std::vector<short>& Grid::getCloseIndexes(int center) const {
	return closeIndexes->get(center);
}

const std::vector<unsigned char>& Grid::getCloseTabIndexes(short center) const {
	return closeIndexes->getTabIndexes(center);
}

bool Grid::onlyOneInside(int index) const {
	return buckets[index].getSize() == 1;
}

void Grid::removeAt(int index, Physical* entity) const {
	if (calculator->isValidIndex(index)) {
		buckets[index].remove(entity);
	}
}

void Grid::addAt(int index, Physical* entity) const {
	buckets[index].add(entity);
}

const std::vector<Physical*>& Grid::getContentAt(int index) const {
	//TODO bug jezeli jest skrajne to bierze z drugiego konca
	if (calculator->isValidIndex(index)) {
		return buckets[index].getContent();
	}
	return Consts::EMPTY_PHYSICAL;
}

const std::vector<Physical*>& Grid::getNotSafeContentAt(short x, short z) const {
	return buckets[calculator->getNotSafeIndex(x, z)].getContent();
}

const std::vector<Physical*>& Grid::getContentAt(short x, short z) const {
	if (calculator->isValidIndex(x, z)) {
		return getNotSafeContentAt(x, z);
	}
	return Consts::EMPTY_PHYSICAL;
}

std::vector<Physical*>* Grid::getArrayNeight(std::pair<Urho3D::Vector3*, Urho3D::Vector3*>& pair,
                                             const char player) {
	invalidateCache();

	const auto posBeginX = calculator->getIndex(std::min(pair.first->x_, pair.second->x_));
	const auto posEndX = calculator->getIndex(std::max(pair.first->x_, pair.second->x_));
	const auto posBeginZ = calculator->getIndex(std::min(pair.first->z_, pair.second->z_));
	const auto posEndZ = calculator->getIndex(std::max(pair.first->z_, pair.second->z_));

	for (short i = posBeginX; i <= posEndX; ++i) {
		for (short j = posBeginZ; j <= posEndZ; ++j) {
			const auto& content = getNotSafeContentAt(i, j);
			std::copy_if(content.begin(), content.end(), std::back_inserter(*tempSelected),
			             [player](Physical* p) { return p->getPlayer() < 0 || p->getPlayer() == player; });
		}
	}

	return tempSelected;
}

std::vector<int> Grid::getArrayNeight(const Urho3D::Vector2& center, float radius) const {
	//TODO clean prawie to samo co wy¿ej
	radius -= 0.1;
	std::vector<int> indexes; //TODO performance

	const auto posBeginX = calculator->getIndex(center.x_ - radius);
	const auto posBeginZ = calculator->getIndex(center.y_ - radius);
	const auto posEndX = calculator->getIndex(center.x_ + radius);
	const auto posEndZ = calculator->getIndex(center.y_ + radius);

	for (short i = posBeginX; i <= posEndX; ++i) {
		for (short j = posBeginZ; j <= posEndZ; ++j) {
			indexes.push_back(calculator->getNotSafeIndex(i, j));
		}
	}

	return indexes;
}

void Grid::invalidateCache() {
	tempSelected->clear();
	prevIndex = -1;
	prevRadius = -1.f;
}

void Grid::invalidateCache(const int currentIdx, float radius) {
	tempSelected->clear();
	prevIndex = currentIdx;
	prevRadius = radius;
}

std::vector<Physical*>* Grid::getArrayNeightSimilarAs(Physical* clicked, float radius) {
	//TODO clean prawie to samo co wy¿ej
	invalidateCache();
	const auto posBeginX = calculator->getIndex(clicked->getPosition().x_ - radius);
	const auto posBeginZ = calculator->getIndex(clicked->getPosition().z_ - radius);
	const auto posEndX = calculator->getIndex(clicked->getPosition().x_ + radius);
	const auto posEndZ = calculator->getIndex(clicked->getPosition().z_ + radius);

	for (short i = posBeginX; i <= posEndX; ++i) {
		for (short j = posBeginZ; j <= posEndZ; ++j) {
			auto& content = getNotSafeContentAt(i, j);
			std::copy_if(content.begin(), content.end(), std::back_inserter(*tempSelected),
			             [clicked](Physical* p) {
				             return p->getId() == clicked->getId() && p->getPlayer() == clicked->getPlayer();
			             });
		}
	}

	return tempSelected;
}

std::vector<Physical*>* Grid::getAllFromCache(int currentIdx, float radius) {
	if (currentIdx == prevIndex && radius == prevRadius) {
		return tempSelected;
	}
	return getAll(currentIdx, radius);
}

std::vector<Physical*>* Grid::getAll(int currentIdx, float radius) {
	invalidateCache(currentIdx, radius);
	for (auto level : *levelCache->get(radius)) {
		auto& content = getContentAt(level + currentIdx);
		tempSelected->insert(tempSelected->end(), content.begin(), content.end());
	}
	return tempSelected;
}
