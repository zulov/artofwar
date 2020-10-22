#include "Grid.h"
#include "Bucket.h"
#include "objects/unit/Unit.h"
#include "simulation/env/ContentInfo.h"
#include "simulation/env/GridCalculatorProvider.h"
#include "utils/consts.h"

Grid::Grid(short resolution, float size): calculator(GridCalculatorProvider::get(resolution, size)), closeIndexProvider(resolution),
                                          resolution(resolution), sqResolution(resolution * resolution),
                                          size(size), fieldSize(size / resolution),
                                          invFieldSize(resolution / size) {

	levelsCache[0] = new std::vector<short>(1);
	for (int i = 1; i < RES_SEP_DIST; ++i) {
		levelsCache[i] = getEnvIndexs((float)MAX_SEP_DIST / RES_SEP_DIST * i, levelsCache[i - 1]);
	}

	buckets = new Bucket[sqResolution];
	tempSelected = new std::vector<Physical*>();
}

Grid::~Grid() {
	delete levelsCache[0];
	for (int i = 1; i < RES_SEP_DIST; ++i) {
		if (levelsCache[i - 1] != levelsCache[i]) {
			delete levelsCache[i];
		}
	}

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

std::vector<short>* Grid::getEnvIndexesFromCache(float dist) {
	const int index = dist * invDiff;
	if (index < RES_SEP_DIST) {
		return levelsCache[index];
	}
	return levelsCache[RES_SEP_DIST - 1];
}

BucketIterator& Grid::getArrayNeight(Urho3D::Vector3& position, float radius, short thread) {
	return *iterators[thread].init(getEnvIndexesFromCache(radius), calculator->indexFromPosition(position), this);
}

const std::vector<short>& Grid::getCloseIndexes(int center) const {
	return closeIndexProvider.get(center);
}

const std::vector<char>& Grid::getCloseTabIndexes(short center) const {
	return closeIndexProvider.getTabIndexes(center);
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

bool Grid::fieldInCircle(short i, short j, float radius) const {
	const short x = i * fieldSize;
	const short y = j * fieldSize;
	return x * x + y * y < radius;
}

std::vector<short>* Grid::getEnvIndexs(float radius, std::vector<short>* prev) const {
	radius *= radius;
	auto indexes = new std::vector<short>();
	for (short i = 0; i < RES_SEP_DIST; ++i) {
		for (short j = 0; j < RES_SEP_DIST; ++j) {
			if (fieldInCircle(i, j, radius)) {
				const short x = i + 1;
				const short y = j + 1;
				indexes->push_back(calculator->getNotSafeIndex(x, y));
				indexes->push_back(calculator->getNotSafeIndex(x, -y));
				indexes->push_back(calculator->getNotSafeIndex(-x, y));
				indexes->push_back(calculator->getNotSafeIndex(-x, -y));
			} else {
				break;
			}
		}
		if (fieldInCircle(i, 0, radius)) {
			const short x = i + 1;
			indexes->push_back(calculator->getNotSafeIndex(x, 0));
			indexes->push_back(calculator->getNotSafeIndex(0, x));
			indexes->push_back(calculator->getNotSafeIndex(-x, 0));
			indexes->push_back(calculator->getNotSafeIndex(0, -x));
		} else {
			break;
		}
	}
	indexes->push_back(calculator->getNotSafeIndex(0, 0));
	std::sort(indexes->begin(), indexes->end());
	if (std::equal(indexes->begin(), indexes->end(), prev->begin(), prev->end())) {
		delete indexes;
		return prev;
	}
	indexes->shrink_to_fit();
	return indexes;
}
