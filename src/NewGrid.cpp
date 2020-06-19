#include "NewGrid.h"

#include <algorithm>
#include "objects/resource/ResourceEntity.h"

template <class T>
NewGrid<T>::NewGrid(short resolution, float size): calculator(resolution, size), closeIndexProvider(resolution),
                                          resolution(resolution), sqResolution(resolution * resolution),
                                          size(size), fieldSize(size / resolution),
                                          invFieldSize(resolution / size) {
	for (int i = 0; i < RES_SEP_DIST; ++i) {
		levelsCache[i] = getEnvIndexs((double)MAX_SEP_DIST / RES_SEP_DIST * i);
	} //TODO memory jesli ten sam vector towskaznik do tego samego

	buckets = new NewBucket<T>[sqResolution];
	tempSelected = new std::vector<T*>();
}

template <class T>
NewGrid<T>::~NewGrid() {
	for (auto& cache : levelsCache) {
		delete cache;
		cache = nullptr;
	}

	delete[] buckets;
	delete tempSelected;
}

// template <class T>
// void NewGrid<T>::update(T* unit, const char team) const {
// 	const int index = calculator.indexFromPosition(unit->getPosition());
//
// 	if (!unit->isAlive()) {
// 		removeAt(unit->getBucketIndex(team), unit);
// 	} else if (unit->bucketHasChanged(index, team)) {
// 		removeAt(unit->getBucketIndex(team), unit);
// 		addAt(index, unit);
// 		unit->setBucket(index, team);
// 	}
// }

template <class T>
void NewGrid<T>::update(T* entity) const {
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

template <class T>
std::vector<short>* NewGrid<T>::getEnvIndexesFromCache(float dist) {
	const int index = dist * invDiff;
	if (index < RES_SEP_DIST) {
		return levelsCache[index];
	}
	return levelsCache[RES_SEP_DIST - 1];
}

template <class T>
NewBucketIterator<T>& NewGrid<T>::getArrayNeight(Urho3D::Vector3& position, float radius, short thread) {
	return *iterators[thread].init(getEnvIndexesFromCache(radius), calculator.indexFromPosition(position), this);
}

template <class T>
const std::vector<short>& NewGrid<T>::getCloseIndexes(int center) const {
	return closeIndexProvider.get(center);
}

template <class T>
const std::vector<char>& NewGrid<T>::getCloseTabIndexes(short center) const {
	return closeIndexProvider.getTabIndexes(center);
}

template <class T>
void NewGrid<T>::removeAt(int index, T* entity) const {
	if (inRange(index)) {
		buckets[index].remove(entity);
	}
}

template <class T>
void NewGrid<T>::addAt(int index, T* entity) const {
	buckets[index].add(entity);
}

template <class T>
std::vector<T*>& NewGrid<T>::getContentAt(int index) {
	if (inRange(index)) {
		return buckets[index].getContent();
	}
	return empty;
}

template <class T>
std::vector<T*>& NewGrid<T>::getContentAt(short x, short z) {
	if (calculator.validIndex(x, z)) {
		return buckets[calculator.getIndex(x, z)].getContent();
	}
	return empty;
}

template <class T>
std::vector<T*>* NewGrid<T>::getArrayNeight(std::pair<Urho3D::Vector3*, Urho3D::Vector3*>& pair, const char player) {
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

template <class T>
std::vector<int> NewGrid<T>::getArrayNeight(const Urho3D::Vector2& center, float radius) const {
	//TODO clean prawie to samo co wy¿ej
	radius -= 0.1;
	std::vector<int> indexes; //TODO performance

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

template <class T>
std::vector<T*>* NewGrid<T>::getArrayNeightSimilarAs(Physical* clicked, double radius) {
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

template <class T>
bool NewGrid<T>::fieldInCircle(short i, short j, float radius) const {
	const short x = i * fieldSize;
	const short y = j * fieldSize;
	return x * x + y * y < radius * radius;
}

template <class T>
std::vector<short>* NewGrid<T>::getEnvIndexs(float radius) const {
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

template class NewGrid<ResourceEntity>;
