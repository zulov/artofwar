#include "MainGrid.h"
#include "BucketQueue.h"
#include "Game.h"
#include "Urho3D/Resource/Image.h"
#include <algorithm>
#include <iomanip>
#include <queue>


MainGrid::MainGrid(const short _resolution, const double _size, const bool _debugEnabled): Grid(_resolution, _size,
                                                                                                _debugEnabled) {
	short posX = 0;
	short posZ = 0;

	tempNeighbour = new std::vector<std::pair<int, float>>();
	tempNeighbour->reserve(DEFAULT_VECTOR_SIZE);
	tempNeighbour2 = new std::vector<std::pair<int, float>>();
	tempNeighbour2->reserve(DEFAULT_VECTOR_SIZE);
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
	delete tempNeighbour;
	delete tempNeighbour2;
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
	tempNeighbour->clear();
	came_from = new int[resolution * resolution];
	cost_so_far = new float[resolution * resolution];
	std::fill_n(came_from, resolution * resolution, -1);
	std::fill_n(cost_so_far, resolution * resolution, -1);
	pathInited = true;
}


bool MainGrid::validateAdd(Static* object) {
	IntVector2 size = object->getGridSize();
	Vector3* pos = object->getPosition();
	return validateAdd(size, *pos);
}

bool MainGrid::validateAdd(const IntVector2& size, Vector3& pos) {
	short iX = getIndex(pos.x_);
	short iZ = getIndex(pos.z_);

	IntVector2 sizeX = calculateSize(size.x_);
	IntVector2 sizeZ = calculateSize(size.y_);

	for (int i = iX + sizeX.x_; i < iX + sizeX.y_; ++i) {
		for (int j = iZ + sizeZ.x_; j < iZ + sizeZ.y_; ++j) {
			const int index = getIndex(i, j);
			if (!(inRange(index) && complexData[index].isUnit())) {
				return false;
			}
		}
	}

	return true;
}

content_info* MainGrid::getContentInfo(const Vector2& from, const Vector2& to, bool checks[], int activePlayer) {
	const short posBeginX = getIndex(from.x_);
	const short posBeginZ = getIndex(from.y_);
	const short posEndX = getIndex(to.x_);
	const short posEndZ = getIndex(to.y_);

	const short iMin = Min(posBeginX, posEndX);
	const short iMax = Max(posBeginX, posEndX);
	const short jMin = Min(posBeginZ, posEndZ);
	const short jMax = Max(posBeginZ, posEndZ);

	ci->reset();

	for (short i = iMin; i < iMax; ++i) {
		for (short j = jMin; j < jMax; ++j) {
			const int index = i * resolution + j;
			updateInfo(index, ci, checks, activePlayer);
		}
	}
	return ci;
}

Vector2& MainGrid::getCenterAt(const IntVector2& cords) {
	const int index = getIndex(cords.x_, cords.y_);
	return complexData[index].getCenter();
}

Vector2 MainGrid::getCenter(int index) {
	return complexData[index].getCenter();
}

void MainGrid::updateInfo(int index, content_info* ci, bool* checks, int activePlayer) {
	switch (complexData[index].getType()) {
	case UNIT:
		{
		if (checks[3] || checks[4]) {
			const bool hasInc = buckets[index].incUnitsPerPlayer(ci, activePlayer, checks);
			if (hasInc) {
				ci->hasUnit = true;
			}
		}
		}
		break;
	case RESOURCE:
		if (checks[1]) {
			ci->hasResource = true;
			ci->resourceNumber[complexData[index].getAdditonalInfo()]++;
		}
		break;
	case BUILDING:
		if (checks[2]) {
			ci->hasBuilding = true;
			ci->buildingNumberPerPlayer[complexData[index].getAdditonalInfo()]++;
		}
		break;
	default: ;
	}


}

IntVector2 MainGrid::calculateSize(int size) {
	int first = -((size - 1) / 2);
	int second = size + first;
	return IntVector2(first, second);
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
	//std::fill_n(came_from + min_cost_to_ref, max_cost_to_ref + 1 - min_cost_to_ref, -1);
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

		IntVector2 sizeX = calculateSize(size.x_);
		IntVector2 sizeZ = calculateSize(size.y_);

		for (int i = iX + sizeX.x_; i < iX + sizeX.y_; ++i) {
			for (int j = iZ + sizeZ.x_; j < iZ + sizeZ.y_; ++j) {
				const int index = getIndex(i, j);
				complexData[index].setStatic(object);
			}
		}

		std::vector<int> toRefresh;

		for (int i = iX + sizeX.x_ - 1; i < iX + sizeX.y_ + 1; ++i) {
			for (int j = iZ + sizeZ.x_ - 1; j < iZ + sizeZ.y_ + 1; ++j) {
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

Vector3* MainGrid::getDirectionFrom(Vector3* position) {
	const short posX = getIndex(position->x_);
	const short posZ = getIndex(position->z_);
	const int index = getIndex(posX, posZ);
	if (!complexData[index].isUnit()) {
		int escapeBucket;
		auto neights = complexData[index].getNeightbours();
		if (!neights.empty()) {
			float dist = (complexData[neights.at(0).first].getCenter() - complexData[index].getCenter()).LengthSquared();
			escapeBucket = neights.at(0).first;

			for (int i = 1; i < neights.size(); ++i) {
				float newDist = (complexData[neights.at(i).first].getCenter() - complexData[index].getCenter()).LengthSquared();
				if (newDist < dist) {
					escapeBucket = neights.at(i).first;
				}
			}
		} else {
			escapeBucket = complexData[index].getEscapeBucket();
		}
		if (escapeBucket == -1) {
			return nullptr;
		}
		Vector3* direction = complexData[index].getDirectrionFrom(position, complexData[escapeBucket]);

		direction->Normalize();
		return direction;
	}
	return nullptr;
}

Vector3* MainGrid::getValidPosition(const IntVector2& size, Vector3* pos) {
	//TODO tu mozna to sporo zoptymalizowac ale pewnie nie ma potrzeby
	const short posX = getIndex(pos->x_);
	const short posZ = getIndex(pos->z_);

	const IntVector2 sizeX = calculateSize(size.x_);
	const IntVector2 sizeZ = calculateSize(size.y_);

	const short left = posX + sizeX.x_;
	const short right = posX + sizeX.y_ - 1;
	const short top = posZ + sizeZ.x_;
	const short bottom = posZ + sizeZ.y_ - 1;
	const int index1 = getIndex(left, top);
	const int index2 = getIndex(right, bottom);
	const Vector2 center1 = complexData[index1].getCenter();
	const Vector2 center2 = complexData[index2].getCenter();
	const Vector2 newCenter = (center1 + center2) / 2;
	pos->x_ = newCenter.x_;
	pos->z_ = newCenter.y_;
	return pos;
}

IntVector2 MainGrid::getBucketCords(const IntVector2& size, Vector3* pos) const {
	return IntVector2(getIndex(pos->x_), getIndex(pos->z_));
}

void MainGrid::updateNeighbors(const int current) {
	tempNeighbour->clear();
	tempNeighbour2->clear();
	IntVector2 cords = getCords(current);
	for (int i = -1; i <= 1; ++i) {
		for (int j = -1; j <= 1; ++j) {
			if (!(i == 0 && j == 0)) {
				if (inSide(cords.x_ + i, cords.y_ + j)) {
					const int index = getIndex(cords.x_ + i, cords.y_ + j);
					if (complexData[index].isUnit()) {
						double costD = cost(current, index);
						tempNeighbour->push_back(std::pair<int, float>(index, costD));
					} else if (!complexData[current].isUnit()) {
						double costD = cost(current, index);
						tempNeighbour2->push_back(std::pair<int, float>(index, costD));
					}
				}
			}
		}
	}
	complexData[current].setNeightbours(tempNeighbour);
	complexData[current].setOccupiedNeightbours(tempNeighbour2);
}

double MainGrid::cost(const int current, const int next) {
	return (complexData[current].getCenter() - complexData[next].getCenter()).Length();
}

void MainGrid::debug(int start, int end) {
	Image* image = new Image(Game::get()->getContext());
	image->SetSize(resolution, resolution, 4);

	String prefix = String(staticCounter) + "_";
	drawMap(image);
	image->SaveBMP("result/images/" + prefix + "1_grid_map.bmp");
	//draw_grid_from(came_from, image);
	//image->SaveBMP("result/images/" + prefix + "2_grid_from.bmp");
	draw_grid_cost(cost_so_far, image);
	image->SaveBMP("result/images/" + prefix + "3_grid_cost.bmp");

	std::vector<int>* path = reconstruct_path(start, end, came_from);
	draw_grid_path(path, image);

	image->SaveBMP("result/images/" + prefix + "4_grid_path.bmp");

	delete image;
	staticCounter++;
}

std::vector<int>* MainGrid::findPath(IntVector2& startV, IntVector2& goalV) {
	int start = getIndex(startV.x_, startV.y_);
	int goal = getIndex(goalV.x_, goalV.y_);
	double min = cost(start, goal);
	return findPath(start, goal, min, min * 2);
}

std::vector<int>* MainGrid::findPath(int startIdx, int endIdx, double min, double max) {
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
	return reconstruct_path(startIdx, endIdx, came_from);
}

std::vector<int>* MainGrid::findPath(int startIdx, const Vector3& aim) {
	const short posX = getIndex(aim.x_);
	const short posZ = getIndex(aim.z_);
	const int end = getIndex(posX, posZ);
	double min = cost(startIdx, end);
	return findPath(startIdx, end, min, min * 2);
}

void MainGrid::refreshWayOut(std::vector<int>& toRefresh) {
	std::set<int> refreshed;
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
				if (!complexData[neight.first].getNeightbours().empty() && refreshed.find(neight.first) == refreshed.end()) {
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

void MainGrid::draw_grid_from(int* cameFrom, Image* image) {
	uint32_t* data = (uint32_t*)image->GetData();
	for (int y = 0; y != resolution; ++y) {
		for (int x = 0; x != resolution; ++x) {
			int id = getIndex(x, y);
			if (cameFrom[id] != -1) {
				IntVector2 cords2 = getCords(cameFrom[id]);
				int x2 = cords2.x_;
				int y2 = cords2.y_;
				int idR = getIndex(resolution - y - 1, x);
				if (x2 == x + 1) {
					*(data + idR) -= 0x00003F00;
				} else if (x2 == x - 1) {
					*(data + idR) -= 0x00007F00;
				} else if (y2 == y + 1) {
					*(data + idR) -= 0x0000BF00;
				} else if (y2 == y - 1) {
					*(data + idR) -= 0x0000FF00;
				}
			}
		}
	}
}

void MainGrid::draw_grid_cost(const float* costSoFar, Image* image) {
	uint32_t* data = (uint32_t*)image->GetData();

	for (short y = 0; y != resolution; ++y) {
		for (short x = 0; x != resolution; ++x) {
			int id = getIndex(x, y);
			if (costSoFar[id] != -1) {
				int idR = getIndex(resolution - y - 1, x);
				*(data + idR) -= 0x0000007F;
			}
		}
	}

}

inline float MainGrid::heuristic(int from, int to) {
	IntVector2 a = getCords(from);
	IntVector2 b = getCords(to);

	return (abs(a.x_ - b.x_) + abs(a.y_ - b.y_)) * fieldSize;
}

void MainGrid::draw_grid_path(std::vector<int>* path, Image* image) {
	uint32_t* data = (uint32_t*)image->GetData();
	for (auto value : *path) {
		IntVector2 a = getCords(value);
		int idR = getIndex(resolution - a.y_ - 1, a.x_);
		*(data + idR) -= 0x0000007F;
	}
}

void MainGrid::drawMap(Image* image) {
	uint32_t* data = (uint32_t*)image->GetData();
	for (short y = 0; y != resolution; ++y) {
		for (short x = 0; x != resolution; ++x) {
			int index = getIndex(x, y);
			int idR = getIndex(resolution - y - 1, x);
			if (complexData[index].isUnit()) {
				*(data + idR) = 0xFFFFFFFF;
			} else {
				*(data + idR) = 0xFF000000;
			}
		}
	}
}

IntVector2 MainGrid::getCords(const int index) {
	return IntVector2(index / resolution, index % resolution);
}

std::vector<int>* MainGrid::reconstruct_path(IntVector2& startV, IntVector2& goalV, const int came_from[]) {
	int start = getIndex(startV.x_, startV.y_);
	int goal = getIndex(goalV.x_, goalV.y_);
	return reconstruct_path(start, goal, came_from);
}

std::vector<int>* MainGrid::reconstruct_path(int start, int goal, const int came_from[]) {
	tempPath->clear();
	int current = goal;
	
	while (current != start) {
		current = came_from[current];
		tempPath->emplace_back(current);
	}
	//path.push_back(start); // optional
	std::reverse(tempPath->begin(), tempPath->end());
	tempPath->emplace_back(goal);
	return tempPath;
}

bool MainGrid::inSide(int x, int z) {
	return !(x < 0 || x >= resolution || z < 0 || z >= resolution);
}
