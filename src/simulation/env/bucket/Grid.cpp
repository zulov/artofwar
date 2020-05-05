#include "Grid.h"
#include "Bucket.h"
#include "BucketIterator.h"
#include "objects/Physical.h"
#include "objects/unit/Unit.h"
#include "simulation/env/ContentInfo.h"
#include <Urho3D/Graphics/Model.h>
#include "math/MathUtils.h"


Grid::Grid(short resolution, float size): calculator(resolution, size), closeIndexProvider(resolution),
                                          resolution(resolution), sqResolution(resolution * resolution),
                                          size(size), fieldSize(size / resolution),
                                          invFieldSize(resolution / size) {
	for (int i = 0; i < RES_SEP_DIST; ++i) {
		levelsCache[i] = getEnvIndexs((double)MAX_SEP_DIST / RES_SEP_DIST * i);
	} //TODO memory jesli ten sam vector towskaznik do tego samego

	buckets = new Bucket[sqResolution];
	tempSelected = new std::vector<Physical*>();
}

Grid::~Grid() {
	for (auto& cache : levelsCache) {
		delete cache;
		cache = nullptr;
	}

	delete[] buckets;
	delete tempSelected;
}

void Grid::update(Unit* unit, const char team) const {
	const int index = calculator.indexFromPosition(unit->getPosition());

	if (!unit->isAlive()) {
		removeAt(unit->getBucketIndex(team), unit);
	} else if (unit->bucketHasChanged(index, team)) {
		removeAt(unit->getBucketIndex(team), unit);
		addAt(index, unit);
		unit->setBucket(index, team);
	}
}

void Grid::update(Physical* entity) const {
	const int index = calculator.indexFromPosition(entity->getPosition());

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

std::vector<short>* Grid::getEnvIndexesFromCache(float dist) {
	const int index = dist * invDiff;
	if (index < RES_SEP_DIST) {
		return levelsCache[index];
	}
	return levelsCache[RES_SEP_DIST - 1];
}

BucketIterator& Grid::getArrayNeight(Urho3D::Vector3& position, float radius, short thread) {
	return *iterators[thread].init(getEnvIndexesFromCache(radius), calculator.indexFromPosition(position), this);
}

const std::vector<short>& Grid::getCloseIndexes(int center) const {
	return closeIndexProvider.get(center);
}

const std::vector<char>& Grid::getCloseTabIndexes(short center) const {
	return closeIndexProvider.getTabIndexes(center);
}

void Grid::removeAt(int index, Physical* entity) const {
	if (inRange(index)) {
		buckets[index].remove(entity);
	}
}

void Grid::addAt(int index, Physical* entity) const {
	buckets[index].add(entity);
}

std::vector<Physical*>& Grid::getContentAt(int index) {
	if (inRange(index)) {
		return buckets[index].getContent();
	}
	return empty;
}

std::vector<Physical*>& Grid::getContentAt(short x, short z) {
	if (calculator.validIndex(x, z)) {
		return buckets[calculator.getIndex(x, z)].getContent();
	}
	return empty;
}

std::vector<Physical*>* Grid::getArrayNeight(std::pair<Urho3D::Vector3*, Urho3D::Vector3*>& pair, const char player) {
	tempSelected->clear();

	const auto posBeginX = calculator.getIndex(std::min(pair.first->x_, pair.second->x_));
	const auto posEndX = calculator.getIndex(std::max(pair.first->x_, pair.second->x_));
	const auto posBeginZ = calculator.getIndex(std::min(pair.first->z_, pair.second->z_));
	const auto posEndZ = calculator.getIndex(std::max(pair.first->z_, pair.second->z_));

	for (short i = posBeginX; i <= posEndX; ++i) {
		for (short j = posBeginZ; j <= posEndZ; ++j) {
			auto& content = getContentAt(i, j);
			std::copy_if(content.begin(), content.end(), std::back_inserter(*tempSelected),
			             [player](Physical* p) { return p->getPlayer() < 0 || p->getPlayer() == player; });
		}
	}

	return tempSelected;
}

std::vector<int> Grid::getArrayNeight(const Urho3D::Vector2& center, float radius) const {
	//TODO clean prawie to samo co wy¿ej
	std::vector<int> indexes;//TODO performance

	const auto posBeginX = calculator.getIndex(center.x_ - radius);
	const auto posBeginZ = calculator.getIndex(center.y_ - radius);
	const auto posEndX = calculator.getIndex(center.x_ + radius);
	const auto posEndZ = calculator.getIndex(center.y_ + radius);

	for (short i = posBeginX; i <= posEndX; ++i) {
		for (short j = posBeginZ; j <= posEndZ; ++j) {
			if (calculator.validIndex(i, j)) {
				indexes.push_back(calculator.getIndex(i, j));
			}
		}
	}

	return indexes;
}

std::vector<Physical*>* Grid::getArrayNeightSimilarAs(Physical* clicked, double radius) {
	//TODO clean prawie to samo co wy¿ej
	tempSelected->clear();

	const auto posBeginX = calculator.getIndex(clicked->getPosition().x_ - radius);
	const auto posBeginZ = calculator.getIndex(clicked->getPosition().z_ - radius);
	const auto posEndX = calculator.getIndex(clicked->getPosition().x_ + radius);
	const auto posEndZ = calculator.getIndex(clicked->getPosition().z_ + radius);


	for (short i = posBeginX; i <= posEndX; ++i) {
		for (short j = posBeginZ; j <= posEndZ; ++j) {
			auto& content = getContentAt(i, j);
			std::copy_if(content.begin(), content.end(), std::back_inserter(*tempSelected),
			             [clicked](Physical* p) {
				             return p->getId() == clicked->getId() && p->getPlayer() == clicked->getPlayer();
			             });
		}
	}

	return tempSelected;
}

bool Grid::fieldInCircle(short i, short j, float radius) const {
	const short x = i * fieldSize;
	const short y = j * fieldSize;
	return x * x + y * y < radius * radius;
}

std::vector<short>* Grid::getEnvIndexs(float radius) const {
	auto indexes = new std::vector<short>();
	for (short i = 0; i < RES_SEP_DIST; ++i) {
		for (short j = 0; j < RES_SEP_DIST; ++j) {
			if (fieldInCircle(i, j, radius)) {
				const short x = i + 1;
				const short y = j + 1;
				indexes->push_back(calculator.getIndex(x, y));
				indexes->push_back(calculator.getIndex(x, -y));
				indexes->push_back(calculator.getIndex(-x, y));
				indexes->push_back(calculator.getIndex(-x, -y));
			} else {
				break;
			}
		}
		if (fieldInCircle(i, 0, radius)) {
			const short x = i + 1;
			indexes->push_back(calculator.getIndex(x, 0));
			indexes->push_back(calculator.getIndex(0, x));
			indexes->push_back(calculator.getIndex(-x, 0));
			indexes->push_back(calculator.getIndex(0, -x));
		} else {
			break;
		}
	}
	indexes->push_back(calculator.getIndex(0, 0));
	std::sort(indexes->begin(), indexes->end());
	indexes->shrink_to_fit();
	return indexes;
}
