#include "MainGrid.h"
#include <xlocale>
#include <queue>
#include <algorithm>
#include <iomanip>
#include "BucketQueue.h"
#include "Urho3D/Resource/Image.h"
#include "Game.h"


MainGrid::MainGrid(const short _resolution, const double _size, const bool _debugEnabled): Grid(_resolution, _size,
                                                                                                _debugEnabled) {
	short posX = 0;
	short posZ = 0;

	const int miniRes = resolution / 16;
	tempNeighbour = new std::vector<std::pair<int, float>*>();
	tempNeighbour->reserve(10);
	const double coef = (miniRes * fieldSize);
	complexData = new ComplexBucketData[resolution * resolution];
	for (int i = 0; i < resolution * resolution; ++i) {
		const double cX = (posX + 0.5) * fieldSize - size / 2;
		const double cZ = (posZ + 0.5) * fieldSize - size / 2;
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
	delete ci;
	if (pathInited) {
		delete[]came_from;
		delete[]cost_so_far;
	}
}

void MainGrid::prepareGridToFind() {
	for (int i = 0; i < resolution * resolution; ++i) {
		tempNeighbour = neighbors(i);
		complexData[i].setNeightbours(tempNeighbour);
	}
	tempNeighbour->clear();
	came_from = new int[resolution * resolution];
	cost_so_far = new float[resolution * resolution];
	pathInited = true;
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

void MainGrid::addStatic(Static* object) {
	if (validateAdd(object)) {
		IntVector2 size = object->getGridSize();
		//Vector3* pos = object->getPosition();
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

		for (int i = iX + sizeX.x_ - 1; i < iX + sizeX.y_ + 1; ++i) {
			for (int j = iZ + sizeZ.x_ - 1; j < iZ + sizeZ.y_ + 1; ++j) {
				const int index = getIndex(i, j);
				tempNeighbour = neighbors(index);
				complexData[index].setNeightbours(tempNeighbour);
			}
		}
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

		Vector3* direction = complexData[index].getDirectrionFrom(position);

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

IntVector2 MainGrid::getBucketCords(const IntVector2& size, Vector3* pos) {
	return IntVector2(getIndex(pos->x_), getIndex(pos->z_));
}

std::vector<std::pair<int, float>*>* MainGrid::neighbors(const int current) {
	tempNeighbour->clear();
	IntVector2 cords = getCords(current);
	for (int i = -1; i <= 1; ++i) {
		for (int j = -1; j <= 1; ++j) {
			if (!(i == 0 && j == 0)) {
				if (inSide(cords.x_ + i, cords.y_ + j)) {
					const int index = getIndex(cords.x_ + i, cords.y_ + j);
					if (complexData[index].isUnit()) {
						double costD = cost(current, index);
						//tempNeighbour->push_back(new std::pair<int, float>(index, costD));
					}
				}
			}
		}
	}
	return tempNeighbour;
}

double MainGrid::cost(const int current, const int next) {
	return (complexData[current].getCenter() - complexData[next].getCenter()).Length();
}

void MainGrid::debug(IntVector2& startV, IntVector2& goalV) {
	Image* image = new Image(Game::get()->getContext());
	image->SetSize(resolution, resolution, 4);

	String prefix = String(staticCounter) + "_";
	drawMap(image);
	image->SaveBMP("result/images/" + prefix + "1_grid_map.bmp");
	//draw_grid_from(came_from, image);
	//image->SaveBMP("result/images/" + prefix + "2_grid_from.bmp");
	draw_grid_cost(cost_so_far, image);
	image->SaveBMP("result/images/" + prefix + "3_grid_cost.bmp");

	std::vector<int> path = reconstruct_path(startV, goalV, came_from);
	draw_grid_path(&path, image);

	image->SaveBMP("result/images/" + prefix + "4_grid_path.bmp");

	delete image;
	staticCounter++;
}

void MainGrid::findPath(IntVector2& startV, IntVector2& goalV) {
	std::fill_n(came_from, resolution * resolution, -1);
	std::fill_n(cost_so_far, resolution * resolution, -1);

	int start = getIndex(startV.x_, startV.y_);
	int goal = getIndex(goalV.x_, goalV.y_);
	double min = cost(start, goal);
	//TODO jak zmieni sie koszt na bardziej skomplikowany to może sie zepsuć a tu ma byćtylko prosta odległość

	//frontier.clear();
	frontier.init(750 + min, min);//TODO ustawić lepsze minimum
	frontier.put(start, 0);

	came_from[start] = start;
	cost_so_far[start] = 0;

	while (!frontier.empty()) {
		const auto current = frontier.get();

		if (current == goal) {
			break;
		}
		auto& neights = complexData[current].getNeightbours();
		for (auto& neight : neights) {
			int next = neight->first;
			if (came_from[current] != next) {
				const float new_cost = cost_so_far[current] + neight->second;
				if (cost_so_far[next] == -1 || new_cost < cost_so_far[next]) {
					cost_so_far[next] = new_cost;
					const float priority = new_cost + heuristic(next, goal);
					frontier.put(next, priority);//TODO a co sie stanie jesliwstawi sie kilka razytego samego noda?
					came_from[next] = current;
				}
			}
		}
	}

	//debug(startV, goalV);
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

inline double MainGrid::heuristic(int from, int to) {
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

std::vector<int> MainGrid::reconstruct_path(IntVector2& startV, IntVector2& goalV, const int came_from[]) {
	int start = getIndex(startV.x_, startV.y_);
	int goal = getIndex(goalV.x_, goalV.y_);
	std::vector<int> path;
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

bool MainGrid::inSide(int x, int z) {
	return !(x < 0 || x >= resolution || z < 0 || z >= resolution);
}
