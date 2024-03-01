#include "PathFinder.h"

#include <chrono>
#include <iostream>
#include <Urho3D/IO/Log.h>

#include "DrawGridUtils.h"
#include "Game.h"
#include "PathCache.h"
#include "../bucket/ComplexBucketData.h"
#include "env/GridCalculator.h"
#include "env/GridCalculatorProvider.h"
#include "env/CloseIndexes.h"
#include "env/CloseIndexesProvider.h"

constexpr char MAX_CACHE_SIZE = 100;

PathFinder::PathFinder(short resolution, float size) :
	closeIndexes(CloseIndexesProvider::get(resolution)), calculator(GridCalculatorProvider::get(resolution, size)),
	resolution(resolution), sqResolution(resolution * resolution), max_cost_to_ref(sqResolution - 1) {
	came_from = new int[sqResolution];
	cost_so_far = new int[sqResolution];
	std::fill_n(came_from, sqResolution, -1);
	std::fill_n(cost_so_far, sqResolution, -1);
	cache = new PathCache[MAX_CACHE_SIZE];
}

PathFinder::~PathFinder() {
	delete[]came_from;
	delete[]cost_so_far;

	delete tempPath;
	delete closePath;
	delete[] cache;
}

void PathFinder::setComplexBucketData(ComplexBucketData* complexData) {
	this->complexData = complexData;
}

const std::vector<int>* PathFinder::reconstructPath(int start, int goal) const {
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

const std::vector<int>* PathFinder::reconstructSimplifyPath(int start, int goal) const {
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

		assert(!(next == current || !calculator->isValidIndex(next)));
		current = next;
	}

	std::ranges::reverse(*tempPath);
	return tempPath;
}

void PathFinder::prepareToStart(int startIdx) {
	resetPathArrays();
	frontier.clear();

	update(startIdx, 0, startIdx, 0);
}

const std::vector<int>* PathFinder::realFindPath(int startIdx, const std::vector<int>& endIdxs, int limit) {
	//performance wersja bez vectora
	assert(limit>0);
	prepareToStart(startIdx);
	auto endCords = getCords(endIdxs);
	assert(!endCords.empty());
	int steps = 0;
	while (!frontier.empty() && ++steps <= limit) {
		const auto current = frontier.get();
		if (std::ranges::binary_search(endIdxs, current)) {
			//debug(startIdx, endIdx);
			return reconstructSimplifyPath(startIdx, current);
		}
		auto const& currentData = complexData[current];
		const auto currentCost = cost_so_far[current];
		const auto currentCameFrom = came_from[current];
		for (auto [i, val] : closeIndexes->getTabIndexesWithValue(currentData)) {
			if (currentData.ifNeightIsFree(i)) {
				int next = current + val;
				assert(validateIndex(current, next));
				if (currentCameFrom != next) {
					const auto new_cost = currentCost + complexData[next].getCost() + distances[i];
					const auto nextCost = cost_so_far[next];
					if (nextCost < 0 || new_cost < nextCost) {
						update(next, new_cost, current,
						       new_cost + heuristic(next, endCords));
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
	const auto foundCacheIdx = findInCache(startIdx, endIdx);
	if (foundCacheIdx > -1) { return &cache[foundCacheIdx].path; }

	if (isInLocalArea(startIdx, endIdx)) {
		closePath->clear();
		closePath->emplace_back(endIdx);
		return closePath;
	}

	const auto closePath = getClosePath2(startIdx, endIdx, closeIndexes->getPassIndexVia1LevelTo2(startIdx, endIdx));
	if (closePath) { return closePath; }

	const std::vector endIdxs = {endIdx};
	const auto path = realFindPath(startIdx, endIdxs, limit);
	addToCache(startIdx, endIdx, path);
	return path;
}

const std::vector<int>*
PathFinder::findPath(int startIdx, const std::vector<int>& endIdxs, int limit) {
	if (endIdxs.empty()) {
		Game::getLog()->WriteRaw("No TargetFound");
		closePath->clear();
		return closePath;
	}

	//TODO perf skorzystac z close indexes?

	return realFindPath(startIdx, endIdxs, limit);
}

bool PathFinder::validateIndex(const int current, int next) const {
	if (calculator->isValidIndex(next)) {
		return true;
	}
	std::cout << current << "@@" << next << std::endl;
	return false;
}

int PathFinder::heuristic(int from, std::vector<Urho3D::IntVector2>& endIdxs) const {
	//bug lepiej wybierac do kogo heurystyka
	assert(!endIdxs.empty());
	const auto a = getCords(from);
	int min = 1024;
	for (const auto& b : endIdxs) {
		min = std::min(min, abs(a.x_ - b.x_) + abs(a.y_ - b.y_));
	}
	return min * PATH_PRECISION;
}

int PathFinder::findInCache(int start, int end) const {
	for (int i = 0; i < cacheSize; ++i) {
		if (cache[i].equal(start, end)) { return i; }
	}
	return -1;
}

void PathFinder::addToCache(int startIdx, int endIdx, const std::vector<int>* vector) {
	cache[cacheIdx].set(startIdx, endIdx, vector);
	++cacheIdx;
	if (cacheIdx >= MAX_CACHE_SIZE) {
		cacheIdx = 0;
	}
	if (cacheSize < MAX_CACHE_SIZE) {
		++cacheSize;
	}
}

void PathFinder::invalidateCache() {
	for (int i = 0; i < cacheSize; ++i) {
		cache[i].clear();
	}
	cacheIdx = 0;
	cacheSize = 0;
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

	auto path = reconstructPath(start, end);
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
			*(data + idR) = complexData[index].isPassable() ? 0xFFFFFFFF : 0xFF000000;
		}
	}
}

void PathFinder::update(int idx, int cost, int cameForm, int heuristicCost) {
	frontier.put(idx, heuristicCost);
	cost_so_far[idx] = cost;
	came_from[idx] = cameForm;
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
	assert(min_cost_to_ref != sqResolution - 1);

	std::fill(cost_so_far + min_cost_to_ref, cost_so_far + max_cost_to_ref + 1, -1);
	std::fill(came_from + min_cost_to_ref, came_from + max_cost_to_ref + 1, -1);

	assert(std::all_of(cost_so_far, cost_so_far + sqResolution, [](int value) { return value == -1; }));
	assert(std::all_of(came_from, came_from + sqResolution, [](int value) { return value == -1; }));

	min_cost_to_ref = sqResolution - 1;
	max_cost_to_ref = 0;
}

bool PathFinder::isInLocalArea(const int center, int indexOfAim) const {
	return closeIndexes->isInLocalArea(center, indexOfAim);
}

bool PathFinder::isInLocal2Area(int center, int indexOfAim) const {
	return closeIndexes->isInLocalLv2Area(center, indexOfAim);
}
