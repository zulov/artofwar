#include "PathFinder.h"

#include <iostream>
#include <Urho3D/IO/Log.h>

#include "DrawGridUtils.h"
#include "Game.h"
#include "../ComplexBucketData.h"
#include "env/GridCalculator.h"
#include "env/GridCalculatorProvider.h"

#include "env/CloseIndexes.h"
#include "env/CloseIndexesProvider.h"

PathFinder::PathFinder(short resolution, float size) :
	closeIndexes(CloseIndexesProvider::get(resolution)), calculator(GridCalculatorProvider::get(resolution, size)),
	resolution(resolution), fieldSize(size / resolution), max_cost_to_ref(resolution * resolution - 1) {
	FibHeap::initCache();

	const auto sqRes = resolution * resolution;
	came_from = new int[sqRes];
	cost_so_far = new float[sqRes];
	std::fill_n(came_from, sqRes, -1);
	std::fill_n(cost_so_far, sqRes, -1.f);
}

PathFinder::~PathFinder() {
	delete[]came_from;
	delete[]cost_so_far;

	delete tempPath;
	delete closePath;
}

void PathFinder::setComplexBucketData(ComplexBucketData* complexData) {
	this->complexData = complexData;
}

const std::vector<int>* PathFinder::reconstructPath(int start, int goal, const int came_from[]) const {
	tempPath->clear();
	int current = goal;

	while (current != start) {
		current = came_from[current];
		tempPath->emplace_back(current);
	}

	std::ranges::reverse(*tempPath);
	tempPath->emplace_back(goal);
	return tempPath;
}

const std::vector<int>* PathFinder::reconstructSimplifyPath(int start, int goal, const int came_from[]) const {
	tempPath->clear();

	int current = goal;

	int lastDirection = 0;
	while (current != start) {
		const int currentDirection = current - came_from[current];
		if (currentDirection != lastDirection) {
			tempPath->emplace_back(current);
			lastDirection = currentDirection;
		}

		int next = came_from[current];

		if (next == current || !calculator->isValidIndex(next)) {
			assert(false);
			break; //TODO BUG tu cos dziwnego sie dzieje
		}
		current = next;
	}

	std::ranges::reverse(*tempPath);
	return tempPath;
}

void PathFinder::prepareToStart(int startIdx) {
	resetPathArrays();
	frontier.clear();
	frontier.put(startIdx, 0.f);

	came_from[startIdx] = startIdx;
	updateCost(startIdx, 0.f);
}

const std::vector<int>* PathFinder::realFindPath(int startIdx, const std::vector<int>& endIdxs, int limit) {
	prepareToStart(startIdx);
	auto endCords = getCords(endIdxs);
	assert(!endCords.empty());

	int steps = 0;
	while (!frontier.empty()) {
		++steps;
		if (limit != -1 && steps > limit) {
			break;
		}
		const auto current = frontier.get();
		if (std::ranges::any_of(endIdxs, [current](int i) { return i == current; })) {
			//debug(startIdx, endIdx);
			return reconstructSimplifyPath(startIdx, current, came_from);
		}
		auto const& currentData = complexData[current];
		for (auto i : closeIndexes->getTabIndexes(current)) {
			if (currentData.ifNeightIsFree(i)) {
				int next = current + closeIndexes->getIndexAt(i);
				assert(validateIndex(current, next));
				if (came_from[current] != next) {
					const float new_cost = cost_so_far[current] + currentData.getCost(i);
					auto const nextCost = cost_so_far[next];
					if (nextCost < 0.f || new_cost < nextCost) {
						updateCost(next, new_cost);
						frontier.put(next, new_cost + heuristic(next, endCords));
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

const std::vector<int>* PathFinder::getClosePath2(int startIdx, int endIdx, const std::vector<short>& closePass) const {
	if (!closePass.empty()) {
		for (auto pass : closePass) {
			auto newPass = pass + startIdx;
			assert(calculator->isValidIndex(newPass) && isInLocalArea(startIdx, newPass));

			if (complexData[newPass].isPassable()) {
				closePath->clear();
				closePath->emplace_back(newPass);
				closePath->emplace_back(endIdx);
				return closePath;
			}
		}
	}
	return nullptr;
}

const std::vector<int>* PathFinder::findPath(int startIdx, int endIdx, int limit) {
	endIdx = getPassableEnd(endIdx); //TODO improve kolejnosc tego  i nize jest istotna?

	if (ifInCache(startIdx, endIdx)) {
		return tempPath;
	}

	if (isInLocalArea(startIdx, endIdx)) {
		closePath->clear();
		closePath->emplace_back(endIdx);
		return closePath;
	}

	const auto closePath = getClosePath2(startIdx, endIdx, closeIndexes->getPassIndexVia1LevelTo2(startIdx, endIdx));
	if (closePath) { return closePath; }

	lastStartIdx = startIdx;
	lastEndIdx = endIdx;
	const std::vector endIdxs = {endIdx};
	return realFindPath(startIdx, endIdxs, limit);
}

const std::vector<int>* PathFinder::findPath(int startIdx, const std::vector<int>& endIdxs, int limit) {
	if (endIdxs.empty()) {
		closePath->clear();
		return closePath;
	}
	if (ifInCache(startIdx, endIdxs)) {
		return tempPath;
	}

	auto newEndIndexes = getPassableIndexes(endIdxs);
	if (newEndIndexes.empty()) {
		Game::getLog()->WriteRaw("No TargetFound");
		closePath->clear();
		return closePath;
	}

	const int localIdx = isInLocalArea(startIdx, newEndIndexes);

	if (localIdx != -1) {
		closePath->clear();
		closePath->emplace_back(localIdx);
		return closePath;
	}
	auto [closePass, endIdx] = closeIndexes->getPassIndexVia1LevelTo2(startIdx, newEndIndexes);
	//TODO zwrócic kilka vectorów??
	const auto closePath = getClosePath2(startIdx, endIdx, closePass);
	if (closePath) { return closePath; }

	const auto path = realFindPath(startIdx, newEndIndexes, limit);
	if (path->empty()) {
		invalidateCache();
	} else {
		lastStartIdx = startIdx;
		lastEndIdx = path->back();
	}
	return path;
}

bool PathFinder::validateIndex(const int current, int next) const {
	if (calculator->isValidIndex(next)) {
		return true;
	}
	std::cout << current << "@@" << next << std::endl;
	return false;
}

int PathFinder::getPassableEnd(int endIdx) const {
	while (!complexData[endIdx].isPassable()) {
		auto& data = complexData[endIdx];
		if (data.allNeightOccupied()) {
			endIdx = data.getEscapeBucket();
			assert(calculator->isValidIndex(endIdx));
		} else {
			for (auto i : closeIndexes->getTabIndexes(endIdx)) {
				if (data.ifNeightIsFree(i)) {
					endIdx = endIdx + closeIndexes->getIndexAt(i); //TODO obliczyc lepszy, a nie pierwszy z brzegu
					break;
				}
			}
		}
	}
	return endIdx;
}

std::vector<int> PathFinder::getPassableIndexes(const std::vector<int>& endIdxs) const {
	std::vector<int> result;
	result.reserve(endIdxs.size());
	for (int endIdx : endIdxs) {
		result.push_back(getPassableEnd(endIdx));
	}
	std::ranges::sort(result);
	result.erase(std::unique(result.begin(), result.end()), result.end());

	return result;
}

void PathFinder::refreshWayOut(std::vector<int>& toRefresh) {
	std::vector<int> refreshed;
	while (!toRefresh.empty()) {
		int startIndex = toRefresh.back();
		toRefresh.pop_back();

		if (std::ranges::find(refreshed, startIndex) != refreshed.end()) {
			continue;
		}
		if (!complexData[startIndex].allNeightOccupied()) {
			complexData[startIndex].setEscapeThrough(-1);
			continue;
		}

		prepareToStart(startIndex);

		int end = startIndex;
		while (!frontier.empty()) {
			const auto current = frontier.get();
			assert(calculator->isValidIndex(current));
			auto& currentData = complexData[current];
			if (!currentData.allNeightOccupied()) {
				end = current;
				currentData.setEscapeThrough(-1);
				break;
			}
			for (auto i : closeIndexes->getTabIndexes(current)) {
				if (!currentData.ifNeightIsFree(i)) {
					int nI = current + closeIndexes->getIndexAt(i);
					assert(calculator->isValidIndex(nI));
					if (!complexData[nI].allNeightOccupied()
						&& std::ranges::find(refreshed, nI) != refreshed.end()) {
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
		const std::vector<int>* path = reconstructPath(startIndex, end, came_from);
		if (!path->empty()) {
			int current2 = startIndex;
			for (int i = 1; i < path->size(); ++i) {
				assert(calculator->isValidIndex(current2));
				complexData[current2].setEscapeThrough(path->at(i));
				refreshed.push_back(current2);
				current2 = path->at(i);
			}
		} else {
			assert(calculator->isValidIndex(startIndex));
			refreshed.push_back(startIndex);
			complexData[startIndex].setEscapeThrough(-1);
		}
	}
}

inline float PathFinder::heuristic(const Urho3D::IntVector2& from, const Urho3D::IntVector2& to) const {
	return (abs(from.x_ - to.x_) + abs(from.y_ - to.y_)) * fieldSize;
}

inline float PathFinder::heuristic(int from, const Urho3D::IntVector2& to) const {
	return heuristic(getCords(from), to);
}

float PathFinder::heuristic(int from, std::vector<Urho3D::IntVector2>& endIdxs) const {
	//closest from endIdxs
	assert(!endIdxs.empty());
	const auto a = getCords(from);
	float min = 1024.f;
	for (auto& endCords : endIdxs) {
		const float val = heuristic(a, endCords);
		if (val < min) {
			min = val;
		}
	}
	return min;
}

bool PathFinder::ifInCache(int startIdx, const std::vector<int>& endIdxs) const {
	if (lastStartIdx != startIdx) { return false; }
	return std::ranges::any_of(endIdxs.begin(), endIdxs.end(),
	                           [this](int i) { return i == lastEndIdx; });
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

	auto path = reconstructPath(start, end, came_from);
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

void PathFinder::updateCost(int idx, float x) {
	cost_so_far[idx] = x;
	if (idx < min_cost_to_ref) {
		min_cost_to_ref = idx;
	}
	if (idx > max_cost_to_ref) {
		max_cost_to_ref = idx;
	}
}

std::vector<Urho3D::IntVector2> PathFinder::getCords(const std::vector<int>& endIdxs) const {
	std::vector<Urho3D::IntVector2> cords;
	cords.reserve(endIdxs.size());
	for (auto idx : endIdxs) {
		cords.emplace_back(getCords(idx));
	}
	return cords;
}

void PathFinder::resetPathArrays() {
	assert(min_cost_to_ref != resolution * resolution - 1);
	//std::fill_n(cost_so_far + min_cost_to_ref, max_cost_to_ref + 1 - min_cost_to_ref, -1.f);
	std::fill(cost_so_far + min_cost_to_ref, cost_so_far + max_cost_to_ref + 1, -1.f);
	//std::fill_n(cost_so_far, resolution * resolution, -1.f);

	int x = -1;
	int y = -1;
	long long val = ((long long)x) << 32 | y;

	memset(came_from + min_cost_to_ref / 2, val, (max_cost_to_ref + 2 - min_cost_to_ref) / 2);
	//std::fill_n(came_from, resolution * resolution, -1);
	min_cost_to_ref = resolution * resolution - 1;
	max_cost_to_ref = 0;
}

bool PathFinder::isInLocalArea(const int center, int indexOfAim) const {
	return closeIndexes->isInLocalArea(center, indexOfAim);
}

bool PathFinder::isInLocal2Area(int center, int indexOfAim) const {
	return closeIndexes->isInLocal2Area(center, indexOfAim);
}

int PathFinder::isInLocalArea(int center, std::vector<int>& endIdxs) const {
	for (auto const idxs : endIdxs) {
		//TOOD perf tu pewnie kilka rzecz sprawdzane pare rzeczy
		if (isInLocalArea(center, idxs)) {
			return idxs;
		}
	}
	return -1;
}
