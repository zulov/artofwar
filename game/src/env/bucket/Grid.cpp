#include "Grid.h"
#include "Bucket.h"
#include "control/MouseButton.h"
#include "levels/LevelCache.h"
#include "levels/LevelCacheProvider.h"
#include "objects/unit/Unit.h"
#include "env/CloseIndexes.h"
#include "env/CloseIndexesProvider.h"
#include "env/ContentInfo.h"
#include "env/GridCalculator.h"
#include "env/GridCalculatorProvider.h"

Grid::Grid(short resolution, float size, float maxQueryRadius)
	: calculator(GridCalculatorProvider::get(resolution, size)),
	  closeIndexes(CloseIndexesProvider::get(resolution)),
	  levelCache(LevelCacheProvider::get(resolution, maxQueryRadius, calculator)),
	  resolution(resolution), sqResolution(resolution * resolution) {
	buckets = new Bucket[sqResolution];
	tempSelected = new std::vector<Physical*>();
	cache = new std::vector<Physical*>();
}

Grid::~Grid() {
	delete[] buckets;
	delete tempSelected;
	delete cache;
}

int Grid::update(Unit* unit, int currentIndex, bool shouldChangeFlag) const {
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
	auto center = calculator->indexFromPosition(position);
	return *iterator.init(levelCache->get(radius, center), center, this);
}

BucketIterator& Grid::getArrayNeight(int center, float radius) {
	return *iterator.init(levelCache->get(radius, center), center, this);
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
	assert(calculator->isValidIndex(index)); 
	return buckets[index].getContent();
}

const std::vector<Physical*>& Grid::getNotSafeContentAt(short x, short z) const {
	return getContentAt(calculator->getNotSafeIndex(x, z));
}

std::vector<Physical*>* Grid::getArrayNeight(MouseHeld& held, const char player) {
	auto [minX, maxX] = calculator->getIndex(held.minMaxX());
	auto [minZ, maxZ] = calculator->getIndex(held.minMaxZ());
	tempSelected->clear();
	for (short i = minX; i <= maxX; ++i) {
		for (short j = minZ; j <= maxZ; ++j) {
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
	//TODO clean prawie to samo co wy�ej
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
	cache->clear();
	prevIndex = currentIdx;
	prevRadius = radius;
}

std::vector<Physical*>* Grid::getArrayNeightSimilarAs(Physical* clicked, float radius) {
	//TODO clean prawie to samo co wy�ej
	const auto posBeginX = calculator->getIndex(clicked->getPosition().x_ - radius);
	const auto posBeginZ = calculator->getIndex(clicked->getPosition().z_ - radius);
	const auto posEndX = calculator->getIndex(clicked->getPosition().x_ + radius);
	const auto posEndZ = calculator->getIndex(clicked->getPosition().z_ + radius);

	tempSelected->clear();
	for (short i = posBeginX; i <= posEndX; ++i) {
		for (short j = posBeginZ; j <= posEndZ; ++j) {
			auto& content = getNotSafeContentAt(i, j);
			std::ranges::copy_if(content, std::back_inserter(*tempSelected),
			                     [clicked](Physical* p) {
				                     return p->getDbId() == clicked->getDbId() && p->getPlayer() == clicked->getPlayer() &&
					                     p->isAlive();
			                     });
		}
	}

	return tempSelected;
}

std::vector<Physical*>* Grid::getAllFromCache(int currentIdx, float radius) {
	if (currentIdx == prevIndex && radius == prevRadius) {
		return cache;
	}
	return getAll(currentIdx, radius);
}

std::vector<Physical*>* Grid::getAll(int currentIdx, float radius) {
	invalidateCache(currentIdx, radius);
	const auto levels = levelCache->get(radius, currentIdx);

	for (const auto idx : *levels) {
		addFromCell(idx, currentIdx);
	}

	return cache;
}

void Grid::addFromCell(short shiftIdx, int currentIdx) const {
	auto& content = getContentAt(shiftIdx + currentIdx);
	cache->insert(cache->end(), content.begin(), content.end());
}
