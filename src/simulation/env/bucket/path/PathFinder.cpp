#include "PathFinder.h"
#include "defines.h"
#include "DrawGridUtils.h"
#include "Game.h"
#include "../ComplexBucketData.h"
#include <Urho3D/Math/Vector2.h>
#include <unordered_set>


PathFinder::PathFinder(short resolution, float size, ComplexBucketData* complexData) : resolution(resolution),
	fieldSize(size / resolution), complexData(complexData), invFieldSize(resolution / size),
	halfResolution(resolution / 2) {
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
			break;
		}
		auto& neights = complexData[current].getNeightbours();
		for (auto& neight : neights) {
			int next = neight.index;
			if (came_from[current] != next) {
				const float new_cost = cost_so_far[current] + neight.cost;
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

std::vector<int>* PathFinder::findPath(int startIdx, const Urho3D::Vector2& aim) {
	int end = getIndex(getIndex(aim.x_), getIndex(aim.y_));

	if (ifInCache(startIdx, end)) {
		return tempPath;
	}

	while (!complexData[end].isUnit()) {
		if (complexData[end].getNeightbours().empty()) {
			end = complexData[end].getEscapeBucket();
		} else {
			end = complexData[end].getNeightbours()[0].index; //TODO obliczyc lepszy
		}
	}

	lastStartIdx = startIdx;
	lastEndIdx = end;

	float min = cost(startIdx, end);
	return findPath(startIdx, end, min, min * 2);
}

float PathFinder::cost(const int current, const int next) const {
	return (complexData[current].getCenter() - complexData[next].getCenter()).Length();
}

void PathFinder::refreshWayOut(std::vector<int>& toRefresh) {
	std::unordered_set<int> refreshed;
	while (!toRefresh.empty()) {
		int startIndex = toRefresh.back();
		toRefresh.pop_back();
		if (refreshed.find(startIndex) != refreshed.end()) {
			continue;
		}
		resetPathArrays();

		frontier.init(750, 0);
		frontier.put(startIndex, 0);

		came_from[startIndex] = startIndex;
		updateCost(startIndex, 0);
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
				if (!complexData[neight.index].getNeightbours().empty() && refreshed.find(neight.index) == refreshed.
					end()) {
					//TODO to chyba glupi warunek
					toRefresh.push_back(neight.index);
				}
				int next = neight.index;
				if (came_from[current] != next) {
					const float new_cost = cost_so_far[current] + neight.cost;
					if (cost_so_far[next] == -1 || new_cost < cost_so_far[next]) {
						updateCost(next, new_cost);

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

inline float PathFinder::heuristic(int from, int to) const {
	auto a = getCords(from);
	auto b = getCords(to);

	return (abs(a.x_ - b.x_) + abs(a.y_ - b.y_)) * fieldSize;
}

void PathFinder::invalidateCache() {
	lastStartIdx = -1;
	lastEndIdx = -1;
}

void PathFinder::debug(int start, int end) {
	Urho3D::Image* image = new Urho3D::Image(Game::getContext());
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

void PathFinder::drawMap(Urho3D::Image* image) {
	const auto data = (uint32_t*)image->GetData();
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

void PathFinder::prepareGridToFind() {
	came_from = new int[resolution * resolution];
	cost_so_far = new float[resolution * resolution];
	std::fill_n(came_from, resolution * resolution, -1);
	std::fill_n(cost_so_far, resolution * resolution, -1);
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
	std::fill_n(cost_so_far + min_cost_to_ref, max_cost_to_ref + 1 - min_cost_to_ref, -1);

	int x = -1;
	int y = -1;
	long long val = ((long long)x) << 32 | y;

	memset(came_from + min_cost_to_ref / 2, val, (max_cost_to_ref + 2 - min_cost_to_ref) / 2);

	min_cost_to_ref = resolution * resolution - 1;
	max_cost_to_ref = 0;
}

short PathFinder::getIndex(float value) const {
	if (value < 0) {
		short index = (short)(value * invFieldSize) + halfResolution - 1;
		if (index >= 0) {
			return index;
		}
		return 0;
	}
	short index = (short)(value * invFieldSize) + halfResolution;
	if (index < resolution) {
		return index;
	}
	return resolution - 1; //TODO czy aby napewno?
}