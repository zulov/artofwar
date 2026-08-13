#include "VisibilityMap.h"
#include <cassert>
#include <algorithm>
#include <limits>
#include <Urho3D/Resource/Image.h>

#include "VisibilityType.h"
#include "Game.h"
#include "debug/DebugLineRepo.h"
#include "colors/ColorPaletteRepo.h"
#include "env/GridCalculatorProvider.h"
#include "env/Environment.h"
#include "env/bucket/levels/LevelCacheProvider.h"
#include "env/influence/VisibilityManager.h"
#include "math/MathUtils.h"
#include "objects/Physical.h"
#include "utils/OtherUtils.h"
#include "utils/SpanUtils.h"
#include "env/bucket/levels/LevelCache.h"
#include "utils/CountUtils.h"

void VisibilityMap::draw() {
	ensureReady();
	DebugLineRepo::drawQuads(DebugLineType::GRID, getResolution(), reinterpret_cast<const unsigned char*>(values),
	                         valueThresholdDebug);
}


VisibilityMap::VisibilityMap(unsigned short resolution, float size, float valueThresholdDebug) :
	calculator(GridCalculatorProvider::get(resolution, size)),
	arraySize(resolution * resolution),
	valueThresholdDebug(valueThresholdDebug),
	influenceRes(resolution / 2),
	influenceArraySize(influenceRes * influenceRes),
	levelCache(LevelCacheProvider::get(resolution, 60.f, calculator)) {
	values = new VisibilityType[arraySize];
	std::fill_n(values, arraySize, VisibilityType::NONE);
	valuesForInfluence = new bool[influenceArraySize];
	std::fill_n(valuesForInfluence, influenceArraySize, false);
	ranges = new float[arraySize];
	std::fill_n(ranges, arraySize, 0.f);
	unseenIntersection.resize(influenceArraySize, 0.f);
}

VisibilityMap::~VisibilityMap() {
	delete[] values;
	delete[] valuesForInfluence;
	delete[] ranges;
}

void VisibilityMap::update(const Urho3D::Vector2& pos, float sRadius) {
	if (sRadius < 0) { return; }
	invalidateCaches();
	const auto centerIdx = calculator->indexFromPosition(pos);
	if (ranges[centerIdx] < sRadius) {
		if (ranges[centerIdx] == 0.f && changedIndexes.size() < CHANGED_INDEXES_MAX_SIZE) {
			changedIndexes.push_back(centerIdx);
		}
		ranges[centerIdx] = sRadius;
	}
}

void VisibilityMap::finishAtIndex(unsigned i) {
	const auto levels = levelCache->get(ranges[i], i);

	for (const auto idx : *levels) {
		const int index = static_cast<int>(i) + idx;
		assert(index >= 0 && index < static_cast<int>(arraySize) &&
		       "out-of-bounds in VisibilityMap::finishAtIndex");
		visibilityValueSum += castC(VisibilityType::VISIBLE) - castC(values[index]);
		values[index] = VisibilityType::VISIBLE;
	}

	ranges[i] = 0.f;
	invalidateCaches();
}

void VisibilityMap::finish() {
	if (changedIndexes.size() >= CHANGED_INDEXES_MAX_SIZE) {
		//full scan
		for (unsigned i = 0; i < arraySize; ++i) {
			if (ranges[i] > 0.f) { finishAtIndex(i); }
		}
	} else { for (const int i : changedIndexes) { finishAtIndex(i); } }

	changedIndexes.clear();
}

void VisibilityMap::reset() {
	invalidateCaches();
	for (auto* value = values; value < values + arraySize; ++value) {
		const auto current = static_cast<unsigned char>(*value);
		visibilityValueSum -= current & 2;
		*value = static_cast<VisibilityType>(current & 1);
	}
}

char VisibilityMap::getValueAt(const Urho3D::Vector2& pos) const {
	return getValueAt(calculator->indexFromPosition(pos));
}

VisibilityType VisibilityMap::getValueAt(float x, float z) const {
	char val = getValueAt(calculator->indexFromPosition(x, z));
	return static_cast<VisibilityType>(val);
}

float VisibilityMap::getValueAt(unsigned index) const {
	assert(index < getResolution() * getResolution());
	return castC(values[index]);
}

int VisibilityMap::removeUnseen(std::span<float> intersection) {
	ensureUnseenIntersectionReady();
	assert(intersection.size() == unseenIntersection.size());
	std::copy_n(unseenIntersection.begin(), unseenIntersection.size(), intersection.begin());
	return visibleCount;
}

float VisibilityMap::getPercent() const {
	return visibilityValueSum / (arraySize * 3.f);
}

void VisibilityMap::ensureReady() {
	if (valuesForInfluenceReady == false) {
		const auto* parent = values;
		auto* current = valuesForInfluence;
		const int res = getResolution();
		for (int prow = 0; prow < res; prow += 2) {
			const auto* row0 = parent + prow * res;
			const auto* row1 = row0 + res;
			auto* dst = current + (prow >> 1) * influenceRes;

			for (auto pcol = 0; pcol < res; pcol += 2, ++dst) {
				*dst =
						row0[pcol] == VisibilityType::VISIBLE ||
						row0[pcol + 1] == VisibilityType::VISIBLE ||
						row1[pcol] == VisibilityType::VISIBLE ||
						row1[pcol + 1] == VisibilityType::VISIBLE;
			}
		}
		valuesForInfluenceReady = true;
	}
}

void VisibilityMap::ensureUnseenIntersectionReady() {
	ensureReady();
	if (unseenIntersectionReady) { return; }

	visibleCount = 0;
	auto* src = valuesForInfluence;
	auto* dst = unseenIntersection.data();
	const auto* const srcEnd = valuesForInfluence + influenceArraySize;
	for (; src < srcEnd; ++src, ++dst) {
		if (*src) {
			*dst = 0.f;
			++visibleCount;
		} else { *dst = std::numeric_limits<float>::max(); }
	}
	unseenIntersectionReady = true;
}

void VisibilityMap::invalidateCaches() const {
	valuesForInfluenceReady = false;
	unseenIntersectionReady = false;
}

Urho3D::Vector3 VisibilityMap::getVertex(const Urho3D::Vector2& center, Urho3D::Vector2 vertex) const {
	auto result = Game::getEnvironment()->getPosWithHeightAt(center.x_ + vertex.x_, center.y_ + vertex.y_);
	result.y_ += 1.f;
	return result;
}

void VisibilityMap::drawCell(unsigned int index) const {}
