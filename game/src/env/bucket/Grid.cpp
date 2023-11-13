#include "Grid.h"
#include "Bucket.h"
#include "levels/LevelCache.h"
#include "levels/LevelCacheProvider.h"
#include "objects/unit/Unit.h"
#include "env/CloseIndexes.h"
#include "env/CloseIndexesProvider.h"
#include "env/ContentInfo.h"
#include "env/GridCalculator.h"
#include "env/GridCalculatorProvider.h"
#include "utils/consts.h"

Grid::Grid(short resolution, float size, bool initCords, float maxQueryRadius)
	: calculator(GridCalculatorProvider::get(resolution, size)),
	  closeIndexes(CloseIndexesProvider::get(resolution)),
	  levelCache(LevelCacheProvider::get(resolution, initCords, maxQueryRadius, calculator)),
	  resolution(resolution), sqResolution(resolution * resolution) {
	buckets = new Bucket[sqResolution];
	tempSelected = new std::vector<Physical*>();
}

Grid::~Grid() {
	delete[] buckets;
	delete tempSelected;
}

int Grid::update(Unit* unit, int currentIndex, bool shouldChangeFlag) const {
	assert(!unit->isToDispose());

	const int index = calculator->indexFromPosition(unit->getPosition());
	const auto areDifferent = currentIndex != index;
	if (areDifferent) {
		removeAt(currentIndex, unit);
		addAt(index, unit);
	}
	if (shouldChangeFlag) {
		unit->setIndexChanged(areDifferent);
	}
	
	return index;
}

int Grid::updateNew(Physical* physical) const {
	//assert(physical->getMainGridIndex(), -1);
	const int index = calculator->indexFromPosition(physical->getPosition());
	addAt(index, physical);
	return index;
}

BucketIterator& Grid::getArrayNeight(const Urho3D::Vector3& position, float radius) {
	return *iterator.init(levelCache->get(radius), calculator->indexFromPosition(position), this);
}

BucketIterator& Grid::getArrayNeight(int center, float radius) {
	return *iterator.init(levelCache->get(radius), center, this);
}

const std::vector<short>& Grid::getCloseIndexes(int center) const {
	return closeIndexes->get(center);
}

bool Grid::onlyOneInside(int index) const {
	return buckets[index].getSize() == 1;
}

void Grid::removeAt(int index, Physical* entity) const {
	assert(index >= 0);
	if (calculator->isValidIndex(index)) {
		buckets[index].remove(entity);
	}
}

void Grid::remove(Physical* physical) const {
	auto index = calculator->indexFromPosition(physical->getPosition());
	buckets[index].remove(physical);
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
			std::ranges::copy_if(content, std::back_inserter(*tempSelected),
			                     [player](Physical* p) {
				                     return (p->getPlayer() < 0 || p->getPlayer() == player) && p->isAlive();
			                     });
		}
	}

	return tempSelected;
}

std::vector<int> Grid::getCloseCenters(Urho3D::Vector2& center, float radius) const {
	//TODO clean prawie to samo co wy¿ej
	radius *= calculator->getFieldSize();
	radius *= 0.5;
	radius -= 0.1;

	const auto posBeginX = calculator->getIndex(center.x_ - radius);
	const auto posBeginZ = calculator->getIndex(center.y_ - radius);
	const auto posEndX = calculator->getIndex(center.x_ + radius);
	const auto posEndZ = calculator->getIndex(center.y_ + radius);
	std::vector<int> indexes; //TODO performance
	indexes.reserve((abs(posEndX - posBeginX) + 1) * (abs(posEndZ - posBeginZ) + 1));
	for (short i = posBeginX; i <= posEndX; ++i) {
		auto index = calculator->getNotSafeIndex(i, posBeginZ);
		for (short j = posBeginZ; j <= posEndZ; ++j) {
			indexes.push_back(index);
			++index;
		}
	}

	return indexes;
}

void Grid::invalidateCache() {
	invalidateCache(-1, -1.f);
}

void Grid::invalidateCache(int currentIdx, float radius) {
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
			std::ranges::copy_if(content, std::back_inserter(*tempSelected),
			                     [clicked](Physical* p) {
				                     return p->getId() == clicked->getId() && p->getPlayer() == clicked->getPlayer() &&
					                     p->isAlive();
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

