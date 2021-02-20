#include "PathFinder.h"
#include <unordered_set>
#include "DrawGridUtils.h"
#include "Game.h"
#include "../ComplexBucketData.h"
#include "simulation/env/GridCalculator.h"
#include "simulation/env/GridCalculatorProvider.h"
#include "utils/defines.h"
#include "simulation/env/CloseIndexes.h"
#include "simulation/env/CloseIndexesProvider.h"

PathFinder::PathFinder(short resolution, float size, ComplexBucketData* complexData) :
	closeIndexes(CloseIndexesProvider::get(resolution)), calculator(GridCalculatorProvider::get(resolution, size)),
	resolution(resolution), fieldSize(size / resolution), max_cost_to_ref(resolution * resolution - 1),
	complexData(complexData) {
	tempPath = new std::vector<int>();
	tempPath->reserve(DEFAULT_VECTOR_SIZE);
}

PathFinder::~PathFinder() {
	delete tempPath;
	if (pathInited) {
		delete[]came_from;
		delete[]cost_so_far;
	}
}

std::vector<int>* PathFinder::reconstruct_path(int start, int goal, const int came_from[]) const {
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

std::vector<int>* PathFinder::reconstruct_simplify_path(int start, int goal, const int came_from[]) const {
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

		int next = came_from[current];

		if (next == current || next < 0) {
			assert(false);
			break; //TODO BUG tu cos dziwnego sie dzieje
		}
		current = next;
	}

	std::reverse(tempPath->begin(), tempPath->end());
	tempPath->pop_back();
	return tempPath;
}

std::vector<int>* PathFinder::findPath(int startIdx, int endIdx, float min, float max) {
	resetPathArrays();
	frontier.init(max, min);
	frontier.put(startIdx, 0);

	came_from[startIdx] = startIdx;

	updateCost(startIdx, 0.0f);
	while (!frontier.empty()) {
		const auto current = frontier.get();

		if (current == endIdx) {
			//debug(startIdx, endIdx);
			return reconstruct_simplify_path(startIdx, endIdx, came_from);
		}
		auto& closeTabIndx = closeIndexes->getTabIndexes(current);
		auto const& currentData = complexData[current];
		for (auto i : closeTabIndx) {
			if (currentData.ifNeightIsFree(i)) {
				int next = current + closeIndexes->getIndexAt(i);
				if (!(next >= 0 && next < resolution * resolution)) {
					std::cout << current << "@@" << next << std::endl;
				}
				assert(next>=0 && next<resolution*resolution);
				if (came_from[current] != next) {
					const float new_cost = cost_so_far[current] + currentData.getCost(i);
					if (cost_so_far[next] < 0.f || new_cost < cost_so_far[next]) {
						updateCost(next, new_cost);
						frontier.put(next, new_cost + heuristic(next, endIdx));
						//TODO next mozna obliczyc raz w srodku
						came_from[next] = current;
					}
				}
			}
		}
	}
	//debug(startIdx, endIdx);
	tempPath->clear();
	return tempPath;
}

std::vector<int>* PathFinder::findPath(const Urho3D::Vector3& from, const Urho3D::Vector2& aim) {
	return findPath(calculator->indexFromPosition(from), aim);
}

int PathFinder::getPassableEnd(int endIdx) const {
	while (!complexData[endIdx].isPassable()) {
		if (complexData[endIdx].allNeightOccupied()) {
			endIdx = complexData[endIdx].getEscapeBucket();
		} else {
			auto& closeTabIndx = closeIndexes->getTabIndexes(endIdx);

			for (auto i : closeTabIndx) {
				if (complexData[endIdx].ifNeightIsFree(i)) {
					endIdx = endIdx + closeIndexes->getIndexAt(i); //TODO obliczyc lepszy, a nie pierwszy z brzegu
					//TODO bug wyjscie pioza
					break;
				}
			}
		}
	}
	return endIdx;
}

std::vector<int>* PathFinder::findPath(int startIdx, int endIdx) {
	if (ifInCache(startIdx, endIdx)) {
		return tempPath;
	}

	endIdx = getPassableEnd(endIdx); //TODO improve kolejnosc tego  i nize jest istotna?

	lastStartIdx = startIdx;
	lastEndIdx = endIdx;

	if (isInLocalArea(startIdx, endIdx)) {
		tempPath->clear();
		tempPath->emplace_back(endIdx);
		return tempPath;
	}

	float min = cost(startIdx, endIdx);
	return findPath(startIdx, endIdx, min, min * 2);
}

std::vector<int>* PathFinder::findPath(int startIdx, const Urho3D::Vector2& aim) {
	return findPath(startIdx, calculator->indexFromPosition(aim));
}

float PathFinder::cost(const int current, const int next) const {
	return calculator->getDistance(current, next);
}

void PathFinder::refreshWayOut(std::vector<int>& toRefresh) {
	std::vector<int> refreshed;
	while (!toRefresh.empty()) {
		int startIndex = toRefresh.back();
		toRefresh.pop_back();

		if (std::find(refreshed.begin(), refreshed.end(), startIndex) != refreshed.end()) {
			continue;
		}
		if (!complexData[startIndex].allNeightOccupied()) {
			complexData[startIndex].setEscapeThrough(-1);
			break;
		}

		resetPathArrays();

		frontier.init(750, 0);
		frontier.put(startIndex, 0);

		came_from[startIndex] = startIndex;
		updateCost(startIndex, 0);
		int end = startIndex;
		while (!frontier.empty()) {
			const auto current = frontier.get();
			auto& currentData = complexData[current];
			if (!currentData.allNeightOccupied()) {
				end = current;
				currentData.setEscapeThrough(-1);
				break;
			}
			auto const& closeTabIndx = closeIndexes->getTabIndexes(current);

			for (auto i : closeTabIndx) {
				if (!currentData.ifNeightIsFree(i)) {
					int nI = current + closeIndexes->getIndexAt(i);
					assert(nI >= 0 && nI < resolution* resolution);
					if (!complexData[nI].allNeightOccupied()
						&& std::find(refreshed.begin(), refreshed.end(), nI) != refreshed.end()) {
						//TODO to chyba glupi warunek
						toRefresh.push_back(nI);
					}
					int next = nI;
					if (came_from[current] != next) {
						const float new_cost = cost_so_far[current] + currentData.getCost(i);
						if (cost_so_far[next] < 0.f || new_cost < cost_so_far[next]) {
							updateCost(next, new_cost);

							frontier.put(next, new_cost);
							came_from[next] = current;
						}
					}
				}
			}
		}
		std::vector<int>* path = reconstruct_path(startIndex, end, came_from);
		if (path->size() >= 1) {
			int current2 = startIndex;
			for (int i = 1; i < path->size(); ++i) {
				complexData[current2].setEscapeThrough(path->at(i));
				refreshed.push_back(current2);
				current2 = path->at(i);
			}
		} else {
			refreshed.push_back(startIndex);
			complexData[startIndex].setEscapeThrough(-1);
		}
	}
}

inline float PathFinder::heuristic(int from, int to) const {
	const auto a = getCords(from);
	const auto b = getCords(to);

	return (abs(a.x_ - b.x_) + abs(a.y_ - b.y_)) * fieldSize;
}

void PathFinder::invalidateCache() {
	lastStartIdx = -1;
	lastEndIdx = -1;
}

void PathFinder::debug(int start, int end) {
	auto image = new Urho3D::Image(Game::getContext());
	image->SetSize(resolution, resolution, 4);

	Urho3D::String prefix = Urho3D::String(staticCounter) + "_";
	drawMap(image);
	image->SaveBMP("result/images/" + prefix + "1_grid_map.bmp");
	//draw_grid_from(came_from, image);
	//image->SaveBMP("result/images/" + prefix + "2_grid_from.bmp");
	draw_grid_cost(cost_so_far, image, resolution);
	image->SaveBMP("result/images/" + prefix + "3_grid_cost.bmp");

	auto path = reconstruct_path(start, end, came_from);
	draw_grid_path(path, image, resolution);

	image->SaveBMP("result/images/" + prefix + "4_grid_path.bmp");

	delete image;
	staticCounter++;
}

void PathFinder::drawMap(Urho3D::Image* image) const {
	const auto data = (uint32_t*)image->GetData();
	for (short y = 0; y != resolution; ++y) {
		for (short x = 0; x != resolution; ++x) {
			const int index = calculator->getIndex(x, y);
			const int idR = calculator->getIndex(resolution - y - 1, x);
			if (complexData[index].isPassable()) {
				*(data + idR) = 0xFFFFFFFF;
			} else {
				*(data + idR) = 0xFF000000;
			}
		}
	}
}

void PathFinder::prepareGridToFind() {
	came_from = new int[resolution * resolution];
	cost_so_far = new float[resolution * resolution];
	std::fill_n(came_from, resolution * resolution, -1);
	std::fill_n(cost_so_far, resolution * resolution, -1.f);
	pathInited = true;
}


void PathFinder::updateCost(int idx, float x) {
	cost_so_far[idx] = x;
	if (idx < min_cost_to_ref) {
		min_cost_to_ref = idx;
	}
	if (idx > max_cost_to_ref) {
		max_cost_to_ref = idx;
	}
}

void PathFinder::resetPathArrays() {
	std::fill_n(cost_so_far + min_cost_to_ref, max_cost_to_ref + 1 - min_cost_to_ref, -1.f);

	int x = -1;
	int y = -1;
	long long val = ((long long)x) << 32 | y;

	memset(came_from + min_cost_to_ref / 2, val, (max_cost_to_ref + 2 - min_cost_to_ref) / 2);

	min_cost_to_ref = resolution * resolution - 1;
	max_cost_to_ref = 0;
}

bool PathFinder::isInLocalArea(const int center, int indexOfAim) const {
	//TODO code duplite
	if (center == indexOfAim) { return true; }
	auto diff = indexOfAim - center; //center + value == indexOfAim
	for (auto value : closeIndexes->get(indexOfAim)) {
		if (diff == value) {
			return true;
		}
	}
	return false;
}
