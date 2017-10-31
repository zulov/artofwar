#include "MainGrid.h"
#include <xlocale>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include "BucketQueue.h"

MainGrid::MainGrid(short _resolution, double _size, bool _debugEnabled): Grid(_resolution, _size, _debugEnabled) {
	short posX = 0;
	short posZ = 0;

	int miniRes = resolution / 16;
	tempNeighbour = new vector<std::pair<int, double>>();
	tempNeighbour->reserve(8);

	for (int i = 0; i < resolution * resolution; ++i) {
		buckets[i].upgrade();

		double cX = (posX + 0.5) * fieldSize - size / 2;
		double cZ = (posZ + 0.5) * fieldSize - size / 2;
		buckets[i].setCenter(cX, cZ);
		if (debugEnabled &&
			(cX > -(miniRes * fieldSize) && cX < (miniRes * fieldSize)) &&
			(cZ > -(miniRes * fieldSize) && cZ < (miniRes * fieldSize))) {
			buckets[i].createBox(fieldSize);
		}
		++posZ;
		if (posZ >= resolution) {
			++posX;
			posZ = 0;
		}
	}

	for (int i = 0; i < resolution * resolution; ++i) {
		tempNeighbour = neighbors(i);
		buckets[i].setNeightbours(tempNeighbour);
	}

	came_from = new int[resolution * resolution];
	cost_so_far = new double[resolution * resolution];
}

MainGrid::~MainGrid() {
	delete tempNeighbour;
	delete[]came_from;
	delete[]cost_so_far;
}

bool MainGrid::validateAdd(Static* object) {
	IntVector2 size = object->getGridSize();
	Vector3* pos = object->getPosition();
	return validateAdd(size, pos);
}

bool MainGrid::validateAdd(const IntVector2& size, Vector3* pos) {
	short iX = getIndex(pos->x_);
	short iZ = getIndex(pos->z_);

	IntVector2 sizeX = calculateSize(size.x_);
	IntVector2 sizeZ = calculateSize(size.y_);

	for (int i = iX + sizeX.x_; i < iX + sizeX.y_; ++i) {
		for (int j = iZ + sizeZ.x_; j < iZ + sizeZ.y_; ++j) {
			const int index = getIndex(i, j);
			if (!(inRange(index) &&
				buckets[index].getType() == ObjectType::UNIT)) {
				return false;
			}
		}
	}

	return true;
}

IntVector2 MainGrid::calculateSize(int size) {
	int first = -((size - 1) / 2);
	int second = size + first;
	return IntVector2(first, second);
}

void MainGrid::addStatic(Static* object) {
	if (validateAdd(object)) {
		IntVector2 size = object->getGridSize();
		Vector3* pos = object->getPosition();
		IntVector2 bucketPos = object->getBucketPosition();
		short iX = bucketPos.x_;
		short iZ = bucketPos.y_;

		object->setBucket(getIndex(iX, iZ), 0);

		IntVector2 sizeX = calculateSize(size.x_);
		IntVector2 sizeZ = calculateSize(size.y_);

		for (int i = iX + sizeX.x_ - 1; i < iX + sizeX.y_ + 1; ++i) {
			for (int j = iZ + sizeZ.x_ - 1; j < iZ + sizeZ.y_ + 1; ++j) {
				const int index = getIndex(i, j);
				tempNeighbour = neighbors(index);
				buckets[index].setNeightbours(tempNeighbour);
			}
		}

		for (int i = iX + sizeX.x_; i < iX + sizeX.y_; ++i) {
			for (int j = iZ + sizeZ.x_; j < iZ + sizeZ.y_; ++j) {
				const int index = getIndex(i, j);
				buckets[index].setStatic(object);
			}
		}
	}
}

void MainGrid::removeStatic(Static* object) {
	//TODO poprawic
	int index = object->getBucketIndex(0);
	buckets[index].removeStatic();
}

Vector3* MainGrid::getDirectionFrom(Vector3* position) {
	short posX = getIndex(position->x_);
	short posZ = getIndex(position->z_);
	const int index = getIndex(posX, posZ);
	if (buckets[index].getType() != ObjectType::UNIT) {

		Vector3* direction = buckets[index].getDirectrionFrom(position);

		direction->Normalize();
		return direction;
	}
	return nullptr;
}

Vector3* MainGrid::getValidPosition(const IntVector2& size, Vector3* pos) {
	//TODO tu mozna to sporo zoptymalizowac ale pewnie nie ma potrzeby
	short posX = getIndex(pos->x_);
	short posZ = getIndex(pos->z_);

	IntVector2 sizeX = calculateSize(size.x_);
	IntVector2 sizeZ = calculateSize(size.y_);

	short left = posX + sizeX.x_;
	short right = posX + sizeX.y_ - 1;
	short top = posZ + sizeZ.x_;
	short bottom = posZ + sizeZ.y_ - 1;
	const int index1 = getIndex(left, top);
	const int index2 = getIndex(right, bottom);
	Vector2 center1 = buckets[index1].getCenter();
	Vector2 center2 = buckets[index2].getCenter();
	Vector2 newCenter = (center1 + center2) / 2;
	pos->x_ = newCenter.x_;
	pos->z_ = newCenter.y_;
	return pos;
}

IntVector2 MainGrid::getBucketCords(const IntVector2& size, Vector3* pos) {
	return IntVector2(getIndex(pos->x_), getIndex(pos->z_));
}

std::vector<std::pair<int, double>>* MainGrid::neighbors(const int current) {
	tempNeighbour->clear();
	IntVector2 cords = getCords(current);
	for (int i = -1; i <= 1; ++i) {
		for (int j = -1; j <= 1; ++j) {
			if (!(i == 0 && j == 0)) {
				if (inSide(cords.x_ + i, cords.y_ + j)) {
					const int index = getIndex(cords.x_ + i, cords.y_ + j);
					if (buckets[index].getType() == UNIT) {
						double costD = cost(current, index);
						tempNeighbour->push_back(pair<int, double>(index, costD));
					}
				}
			}
		}
	}
	return tempNeighbour;
}

double MainGrid::cost(const int current, const int next) {
	double result = (buckets[current].getCenter() - buckets[next].getCenter()).Length();
	return result;
}

void MainGrid::findPath(IntVector2& startV, IntVector2& goalV) {
	std::fill_n(came_from, resolution * resolution, -1);
	std::fill_n(cost_so_far, resolution * resolution, -1);

	int start = getIndex(startV.x_, startV.y_);
	int goal = getIndex(goalV.x_, goalV.y_);
	double min = cost(start, goal);
	//TODO jak zmieni sie koszt na bardziej skomplikowany to może sie zepsuć a tu ma byćtylko prosta odległość
	//PriorityQueue frontier;
	//FibHeap frontier;

	frontier.init(1000 + min, min);//TODO ustawić lepsze minimum
	frontier.put(start, 0);

	came_from[start] = start;
	cost_so_far[start] = 0;

	while (!frontier.empty()) {
		const auto current = frontier.get();

		if (current == goal) {
			break;
		}

		for (auto& neight : buckets[current].getNeightbours()) {
			int next = neight.first;
			if (came_from[current] != next) {
				const double new_cost = cost_so_far[current] + neight.second;
				if (cost_so_far[next] == -1 || new_cost < cost_so_far[next]) {
					cost_so_far[next] = new_cost;
					const double priority = new_cost + heuristic(next, goal);
					frontier.put(next, priority);
					came_from[next] = current;
				}
			}
		}
	}
}

void MainGrid::draw_grid_from(int* cameFrom) {
	for (int y = 0; y != resolution; ++y) {
		for (int x = 0; x != resolution; ++x) {
			int id = getIndex(x, y);
			if (cameFrom[id] != -1) {
				IntVector2 cords2 = getCords(cameFrom[id]);
				int x2 = cords2.x_;
				int y2 = cords2.y_;

				if (x2 == x + 1) { std::wcout << ">"; } else if (x2 == x - 1) { std::wcout << "<"; } else if (y2 == y + 1) {
					std::wcout << "u";
				} else if (y2 == y - 1) { std::wcout << "d"; } else { std::wcout << "*"; }

			} else {
				drawEmpty(id);
			}

		}
		std::cout << std::endl;
	}
}

void MainGrid::draw_grid_cost(double* costSoFar) {
	for (int y = 0; y != resolution; ++y) {
		for (int x = 0; x != resolution; ++x) {
			int id = getIndex(x, y);
			if (costSoFar[id] != -1) {
				std::wcout << "|" << costSoFar[id];
			} else {
				drawEmpty(id);
			}

		}
		std::cout << std::endl;
	}
}

inline double MainGrid::heuristic(int from, int to) {
	IntVector2 a = getCords(from);
	IntVector2 b = getCords(to);
	return (abs(a.x_ - b.x_) + abs(a.y_ - b.y_)) * fieldSize;
}

void MainGrid::draw_grid_path(vector<int>* path) {
	for (int y = 0; y != resolution; ++y) {

		for (int x = 0; x != resolution; ++x) {
			int id = getIndex(x, y);

			if (find(path->begin(), path->end(), id) != path->end()) {
				std::wcout << '@';
			} else {
				drawEmpty(id);
			}
		}
		std::cout << std::endl;
	}
}

IntVector2 MainGrid::getCords(const int index) {
	return IntVector2(index / resolution, index % resolution);
}

vector<int> MainGrid::reconstruct_path(IntVector2& startV, IntVector2& goalV, int came_from[]) {
	int start = getIndex(startV.x_, startV.y_);
	int goal = getIndex(goalV.x_, goalV.y_);
	vector<int> path;
	int current = goal;
	path.push_back(current);
	while (current != start) {
		current = came_from[current];
		path.push_back(current);
	}
	path.push_back(start); // optional
	std::reverse(path.begin(), path.end());
	return path;
}

void MainGrid::drawEmpty(int id) {
	if (buckets[id].getType() == UNIT) {
		std::wcout << '.';
	} else {
		std::wcout << '#';
	}
}


bool MainGrid::inSide(int x, int z) {
	if (x < 0 || x >= resolution || z < 0 || z >= resolution) {
		return false;
	}
	return true;
}
