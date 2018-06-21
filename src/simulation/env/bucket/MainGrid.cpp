#include "MainGrid.h"
#include "BucketQueue.h"
#include "Game.h"
#include "DrawGridUtils.h"
#include "Urho3D/Resource/Image.h"
#include "objects/Static.h"
#include "objects/unit/Unit.h"
#include "Bucket.h"
#include <algorithm>
#include <iomanip>
#include <unordered_set>
#include "simulation/env/ContentInfo.h"
#include "MathUtils.h"
#include <array>


MainGrid::MainGrid(const short _resolution, const float _size, const bool _debugEnabled): Grid(_resolution, _size,
                                                                                               _debugEnabled) {
	short posX = 0;
	short posZ = 0;

	tempPath = new std::vector<int>();
	tempPath->reserve(DEFAULT_VECTOR_SIZE);
	const float coef = (resolution / 16) * fieldSize;

	complexData = new ComplexBucketData[resolution * resolution];
	for (int i = 0; i < resolution * resolution; ++i) {
		const float cX = (posX + 0.5) * fieldSize - size / 2;
		const float cZ = (posZ + 0.5) * fieldSize - size / 2;
		complexData[i].setCenter(cX, cZ);
		if (debugEnabled &&
			(cX > -coef && cX < coef) &&
			(cZ > -coef && cZ < coef)) {
			complexData[i].createBox(fieldSize);
		}
		++posZ;
		if (posZ >= resolution) {
			++posX;
			posZ = 0;
		}
	}

	ci = new content_info();
}

MainGrid::~MainGrid() {
	delete[] complexData;
	delete tempPath;
	delete ci;
	if (pathInited) {
		delete[]came_from;
		delete[]cost_so_far;
	}
}

void MainGrid::prepareGridToFind() {
	for (int i = 0; i < resolution * resolution; ++i) {
		updateNeighbors(i);
	}

	came_from = new int[resolution * resolution];
	cost_so_far = new float[resolution * resolution];
	std::fill_n(came_from, resolution * resolution, -1);
	std::fill_n(cost_so_far, resolution * resolution, -1);
	pathInited = true;
}


bool MainGrid::validateAdd(Static* object) {
	Vector3* pos = object->getPosition();
	return validateAdd(object->getGridSize(), Vector2(pos->x_, pos->z_));
}

bool MainGrid::validateAdd(const IntVector2& size, Vector2& pos) {
	const IntVector2 sizeX = calculateSize(size.x_, getIndex(pos.x_));
	const IntVector2 sizeZ = calculateSize(size.y_, getIndex(pos.y_));

	for (int i = sizeX.x_; i < sizeX.y_; ++i) {
		for (int j = sizeZ.x_; j < sizeZ.y_; ++j) {
			const int index = getIndex(i, j);
			if (!(inRange(index) && complexData[index].isUnit())) {
				return false;
			}
		}
	}

	return true;
}

content_info* MainGrid::getContentInfo(const Vector2& from, const Vector2& to, bool checks[], int activePlayer) {
	const auto posBeginX = getIndex(from.x_);
	const auto posBeginZ = getIndex(from.y_);
	const auto posEndX = getIndex(to.x_);
	const auto posEndZ = getIndex(to.y_);

	const auto iMin = Min(posBeginX, posEndX);
	const auto iMax = Max(posBeginX, posEndX);
	const auto jMin = Min(posBeginZ, posEndZ);
	const auto jMax = Max(posBeginZ, posEndZ);

	ci->reset();

	for (short i = iMin; i < iMax; ++i) {
		for (short j = jMin; j < jMax; ++j) {
			const int index = i * resolution + j;
			updateInfo(index, ci, checks, activePlayer);
		}
	}
	return ci;
}

Vector2 MainGrid::repulseObstacle(Unit* unit) {
	auto index = indexFromPosition(unit->getPosition());

	Vector2 sum;
	if (complexData[index].isUnit()
		&& !complexData[index].getOccupiedNeightbours().empty()) {
		for (const auto neightbour : complexData[index].getOccupiedNeightbours()) {
			sum += complexData[neightbour.first].getCenter();
		}
		sum /= complexData[index].getOccupiedNeightbours().size();
		sum -= Vector2(unit->getPosition()->x_, unit->getPosition()->z_);

	}
	return -sum;
}

void MainGrid::invalidateCache() {
	lastStartIdx = -1;
	lastEndIdx = -1;
}

void MainGrid::updateSurround(ResourceEntity* resource) {
	std::unordered_set<int> indexes;
	std::array<short, 8> closeIndex = {-513, -512, -511, -1, 1, 511, 512, 513};
	for (auto index : resource->getOcupiedCells()) {
		for (auto inIndex : closeIndex) {
			auto newIndex = index + inIndex;
			indexes.emplace(newIndex);
		}
	}
	for (auto index : resource->getOcupiedCells()) {
		indexes.erase(index);
	}
	auto& cells = resource->getSurroundCells();
	for (auto index : indexes) {//TODO dodac na raz
		cells.emplace_back(index);
	}
}

void MainGrid::updateInfo(int index, content_info* ci, bool* checks, int activePlayer) {
	switch (complexData[index].getType()) {
	case ObjectType::UNIT:
		{
		if (checks[3] || checks[4]) {
			const bool hasInc = buckets[index].incUnitsPerPlayer(ci, activePlayer, checks);
			if (hasInc) {
				ci->hasUnit = true;
			}
		}
		}
		break;
	case ObjectType::RESOURCE:
		if (checks[1]) {
			ci->hasResource = true;
			ci->resourceNumber[complexData[index].getAdditonalInfo()]++;
		}
		break;
	case ObjectType::BUILDING:
		if (checks[2]) {
			ci->hasBuilding = true;
			ci->buildingNumberPerPlayer[complexData[index].getAdditonalInfo()]++;
		}
		break;
	default: ;
	}
}

void MainGrid::updateCost(int idx, float x) {
	cost_so_far[idx] = x;
	if (idx < min_cost_to_ref) {
		min_cost_to_ref = idx;
	}
	if (idx > max_cost_to_ref) {
		max_cost_to_ref = idx;
	}
}

void MainGrid::resetPathArrays() {
	std::fill_n(cost_so_far + min_cost_to_ref, max_cost_to_ref + 1 - min_cost_to_ref, -1);

	int x = -1;
	int y = -1;
	long long val = ((long long)x) << 32 | y;

	memset(came_from + min_cost_to_ref / 2, val, (max_cost_to_ref + 2 - min_cost_to_ref) / 2);

	min_cost_to_ref = resolution * resolution - 1;
	max_cost_to_ref = 0;
}

void MainGrid::addStatic(Static* object) {
	if (validateAdd(object)) {
		IntVector2 size = object->getGridSize();

		IntVector2 bucketPos = object->getBucketPosition();
		short iX = bucketPos.x_;
		short iZ = bucketPos.y_;

		object->setBucket(getIndex(iX, iZ), 0);

		const IntVector2 sizeX = calculateSize(size.x_, iX);
		const IntVector2 sizeZ = calculateSize(size.y_, iZ);

		for (int i = sizeX.x_; i < sizeX.y_; ++i) {
			for (int j = sizeZ.x_; j < sizeZ.y_; ++j) {
				const int index = getIndex(i, j);
				complexData[index].setStatic(object);
			}
		}

		std::vector<int> toRefresh;

		for (int i = sizeX.x_ - 1; i < sizeX.y_ + 1; ++i) {
			for (int j = sizeZ.x_ - 1; j < sizeZ.y_ + 1; ++j) {
				const int index = getIndex(i, j);
				updateNeighbors(index);
				if (!complexData[index].isUnit()) {
					toRefresh.push_back(index);
				} else {
					complexData[index].setEscapeThrought(-1);
				}
			}
		}
		refreshWayOut(toRefresh);
	}
}

void MainGrid::removeStatic(Static* object) {
	//TODO poprawic
	int index = object->getBucketIndex(0);
	complexData[index].removeStatic();
}

Vector2* MainGrid::getDirectionFrom(Vector3* position) {
	int index = indexFromPosition(position);
	if (!complexData[index].isUnit()) {
		int escapeBucket;
		auto& neights = complexData[index].getNeightbours();
		if (!neights.empty()) {
			float dist = (complexData[neights.at(0).first].getCenter() - complexData[index].getCenter()).
				LengthSquared();
			escapeBucket = neights.at(0).first;

			for (int i = 1; i < neights.size(); ++i) {
				float newDist = (complexData[neights.at(i).first].getCenter() - complexData[index].getCenter()).
					LengthSquared();
				if (newDist < dist) {
					escapeBucket = neights.at(i).first;
				}
			}
		} else {
			escapeBucket = complexData[index].getEscapeBucket();
		}
		if (escapeBucket == -1) {
			return new Vector2;
		}
		Vector2* direction = complexData[index].getDirectrionFrom(position, complexData[escapeBucket]);

		direction->Normalize();
		return direction;
	}
	return nullptr;
}

Vector2 MainGrid::getValidPosition(const IntVector2& size, const Vector2& pos) {
	//TODO tu mozna to sporo zoptymalizowac ale pewnie nie ma potrzeby
	const short posX = getIndex(pos.x_);
	const short posZ = getIndex(pos.y_);

	const IntVector2 sizeX = calculateSize(size.x_, posX);
	const IntVector2 sizeZ = calculateSize(size.y_, posZ);

	const int index1 = getIndex(sizeX.x_, sizeZ.x_);
	const int index2 = getIndex(sizeX.y_ - 1, sizeZ.y_ - 1);
	const Vector2 center1 = complexData[index1].getCenter();
	const Vector2 center2 = complexData[index2].getCenter();
	const Vector2 newCenter = (center1 + center2) / 2;

	return newCenter;
}

IntVector2 MainGrid::getBucketCords(const IntVector2& size, const Vector2& pos) const {
	return IntVector2(getIndex(pos.x_), getIndex(pos.y_));
}

void MainGrid::updateNeighbors(const int current) const {
	auto& neigths = complexData[current].getNeightbours();
	neigths.clear();
	auto& ocupNeigths = complexData[current].getOccupiedNeightbours();
	ocupNeigths.clear();

	IntVector2 cords = getCords(current);
	for (int i = -1; i <= 1; ++i) {
		for (int j = -1; j <= 1; ++j) {
			if (!(i == 0 && j == 0)) {
				if (inSide(cords.x_ + i, cords.y_ + j)) {
					const int index = getIndex(cords.x_ + i, cords.y_ + j);
					if (complexData[index].isUnit()) {
						neigths.emplace_back(index, cost(current, index));
					} else if (!complexData[index].isUnit()) {
						ocupNeigths.emplace_back(index, cost(current, index));
					}
				}
			}
		}
	}
}

float MainGrid::cost(const int current, const int next) const {
	return (complexData[current].getCenter() - complexData[next].getCenter()).Length();
}

void MainGrid::debug(int start, int end) {
	Image* image = new Image(Game::getContext());
	image->SetSize(resolution, resolution, 4);

	String prefix = String(staticCounter) + "_";
	drawMap(image);
	image->SaveBMP("result/images/" + prefix + "1_grid_map.bmp");
	//draw_grid_from(came_from, image);
	//image->SaveBMP("result/images/" + prefix + "2_grid_from.bmp");
	draw_grid_cost(cost_so_far, image, resolution);
	image->SaveBMP("result/images/" + prefix + "3_grid_cost.bmp");

	std::vector<int>* path = reconstruct_path(start, end, came_from);
	draw_grid_path(path, image, resolution);

	image->SaveBMP("result/images/" + prefix + "4_grid_path.bmp");

	delete image;
	staticCounter++;
}

std::vector<int>* MainGrid::findPath(IntVector2& startV, IntVector2& goalV) {
	int start = getIndex(startV.x_, startV.y_);
	int goal = getIndex(goalV.x_, goalV.y_);
	float min = cost(start, goal);
	return findPath(start, goal, min, min * 2);
}

std::vector<int>* MainGrid::findPath(int startIdx, int endIdx, float min, float max) {
	resetPathArrays();
	frontier.init(max, min);
	frontier.put(startIdx, 0);

	came_from[startIdx] = startIdx;

	updateCost(startIdx, 0.0f);
	while (!frontier.empty()) {
		const auto current = frontier.get();

		if (current == endIdx) {
			break;
		}
		auto& neights = complexData[current].getNeightbours();
		for (auto& neight : neights) {
			int next = neight.first;
			if (came_from[current] != next) {
				const float new_cost = cost_so_far[current] + neight.second;
				if (cost_so_far[next] == -1 || new_cost < cost_so_far[next]) {

					updateCost(next, new_cost);
					frontier.put(next, new_cost + heuristic(next, endIdx));
					came_from[next] = current;
				}
			}
		}
	}
	//debug(startIdx, endIdx);
	return reconstruct_simplify_path(startIdx, endIdx, came_from);
}

std::vector<int>* MainGrid::findPath(int startIdx, const Vector2& aim) {
	int end = getIndex(getIndex(aim.x_), getIndex(aim.y_));

	if (ifInCache(startIdx, end)) {
		return tempPath;
	}

	while (!complexData[end].isUnit()) {
		if (complexData[end].getNeightbours().empty()) {
			end = complexData[end].getEscapeBucket();
		} else {
			end = complexData[end].getNeightbours()[0].first; //TODO obliczyc lepszy
		}
	}

	lastStartIdx = startIdx;
	lastEndIdx = end;

	float min = cost(startIdx, end);
	return findPath(startIdx, end, min, min * 2);
}

void MainGrid::refreshWayOut(std::vector<int>& toRefresh) {
	std::unordered_set<int> refreshed;
	while (!toRefresh.empty()) {
		int startIndex = toRefresh.back();
		toRefresh.pop_back();
		if (refreshed.find(startIndex) != refreshed.end()) {
			continue;
		}
		std::fill_n(came_from, resolution * resolution, -1);
		std::fill_n(cost_so_far, resolution * resolution, -1);

		frontier.init(750, 0);
		frontier.put(startIndex, 0);

		came_from[startIndex] = startIndex;
		cost_so_far[startIndex] = 0;
		int end = startIndex;
		while (!frontier.empty()) {
			const auto current = frontier.get();

			if (!complexData[current].getNeightbours().empty()) {
				end = current;
				complexData[current].setEscapeThrought(-1);
				break;
			}
			auto& neights = complexData[current].getOccupiedNeightbours();
			for (auto& neight : neights) {
				if (!complexData[neight.first].getNeightbours().empty() && refreshed.find(neight.first) == refreshed.
					end()) {
					//TODO to chyba glupi warunek
					toRefresh.push_back(neight.first);
				}
				int next = neight.first;
				if (came_from[current] != next) {
					const float new_cost = cost_so_far[current] + neight.second;
					if (cost_so_far[next] == -1 || new_cost < cost_so_far[next]) {
						cost_so_far[next] = new_cost;

						frontier.put(next, new_cost);
						came_from[next] = current;
					}
				}
			}
		}
		std::vector<int>* path = reconstruct_path(startIndex, end, came_from);
		if (path->size() >= 1) {
			int current2 = startIndex;
			for (int i = 1; i < path->size(); ++i) {
				complexData[current2].setEscapeThrought(path->at(i));
				refreshed.insert(current2);
				current2 = path->at(i);
			}
		} else {
			refreshed.insert(startIndex);
			complexData[startIndex].setEscapeThrought(-1);
		}

	}

}

inline float MainGrid::heuristic(int from, int to) {
	IntVector2 a = getCords(from);
	IntVector2 b = getCords(to);

	return (abs(a.x_ - b.x_) + abs(a.y_ - b.y_)) * fieldSize;
}

void MainGrid::drawMap(Image* image) {
	uint32_t* data = (uint32_t*)image->GetData();
	for (short y = 0; y != resolution; ++y) {
		for (short x = 0; x != resolution; ++x) {
			const int index = getIndex(x, y);
			const int idR = getIndex(resolution - y - 1, x);
			if (complexData[index].isUnit()) {
				*(data + idR) = 0xFFFFFFFF;
			} else {
				*(data + idR) = 0xFF000000;
			}
		}
	}
}

std::vector<int>* MainGrid::reconstruct_path(IntVector2& startV, IntVector2& goalV, const int came_from[]) {
	return reconstruct_path(getIndex(startV.x_, startV.y_),
	                        getIndex(goalV.x_, goalV.y_),
	                        came_from);
}

std::vector<int>* MainGrid::reconstruct_path(int start, int goal, const int came_from[]) {
	tempPath->clear();
	int current = goal;

	while (current != start) {
		current = came_from[current];
		tempPath->emplace_back(current);
	}

	std::reverse(tempPath->begin(), tempPath->end());
	tempPath->emplace_back(goal);
	return tempPath;
}

std::vector<int>* MainGrid::reconstruct_simplify_path(int start, int goal, const int came_from[]) {
	tempPath->clear();

	tempPath->emplace_back(goal);
	int current = goal;

	int lastDirection = 0;
	while (current != start) {

		const int currentDirection = current - came_from[current];
		if (currentDirection != lastDirection) {
			tempPath->emplace_back(current);
			lastDirection = currentDirection;
		}
		current = came_from[current];
	}

	std::reverse(tempPath->begin(), tempPath->end());
	tempPath->pop_back();
	return tempPath;
}
