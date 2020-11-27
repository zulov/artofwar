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
	  resolution(resolution), sqResolution(resolution * resolution){
	buckets = new Bucket[sqResolution];
	tempSelected = new std::vector<Physical*>();
}

Grid::~Grid() {
	delete[] buckets;
	delete tempSelected;
}

void Grid::update(Unit* unit, const char team) const {
	const int index = calculator->indexFromPosition(unit->getPosition());

	if (!unit->isAlive()) {
		removeAt(unit->getBucketIndex(team), unit);
	} else if (unit->bucketHasChanged(index, team)) {
		removeAt(unit->getBucketIndex(team), unit);
		addAt(index, unit);
		unit->setBucket(index, team);
	}
}

void Grid::update(Physical* entity) const {
	const int index = calculator->indexFromPosition(entity->getPosition());

	if (!entity->isAlive()) {
		removeAt(entity->getMainBucketIndex(), entity);
	} else if (entity->bucketHasChanged(index)) {
		removeAt(entity->getMainBucketIndex(), entity);
		addAt(index, entity);
		entity->setBucket(index);
	} else {
		entity->indexHasChangedReset();
	}
}

BucketIterator& Grid::getArrayNeight(Urho3D::Vector3& position, float radius) {
	return *iterator.init(levelCache->get(radius), calculator->indexFromPosition(position), this);
}

const std::vector<short>& Grid::getCloseIndexes(int center) const {
	return closeIndexes->get(center);
}

const std::vector<char>& Grid::getCloseTabIndexes(short center) const {
	return closeIndexes->getTabIndexes(center);
}

bool Grid::isSameBucket(const int prevIdx, const Urho3D::Vector3& vector3) const {
	return prevIdx==calculator->indexFromPosition(vector3);
}

int Grid::getIndexFromPositions(const Urho3D::Vector3& vector3) {
	return calculator->indexFromPosition(vector3);
}

bool Grid::onlyOneInside(int test) {
	return buckets[test].getSize()==1;
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
                                             const char player) const {
	tempSelected->clear();

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

std::vector<Physical*>* Grid::getArrayNeightSimilarAs(Physical* clicked, float radius) {
	//TODO clean prawie to samo co wy¿ej
	tempSelected->clear();

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
