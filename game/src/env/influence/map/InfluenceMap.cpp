#include "InfluenceMap.h"

#include <algorithm>
#include <cassert>
#include <numeric>
#include <ranges>

#include "env/influence/MapsUtils.h"
#include "env/influence/map/InfluenceTemplateProvider.h"
#include "env/bucket/ArrayProviderUtils.h"
#include "math/MathUtils.h"
#include "math/VectorUtils.h"
#include "utils/SpanUtils.h"

namespace {
	constexpr float KERNEL_COEF = 0.5f;
	constexpr float HISTORY_MINIMAL_THRESHOLD = 0.0001f;
	constexpr float HISTORY_VANISH_COEF = 0.5f;
	constexpr std::size_t MAX_NON_ZERO_INDEXES = 100;
	constexpr std::size_t MAX_PENDING_NON_ZERO_INDEXES = 75;

	void addNonZeroIndex(std::vector<unsigned>& nonZeroIndexes, unsigned index) {
		if (nonZeroIndexes.size() < MAX_NON_ZERO_INDEXES) { nonZeroIndexes.push_back(index); }
	}
}

InfluenceMap::InfluenceMap(GridCalculator* calculator, float valueThresholdDebug, bool history) :
	arraySize(calculator->getResolution() * calculator->getResolution()),
	calculator(calculator), debugThreshold(valueThresholdDebug),
	templateV(InfluenceTemplateProvider::get(KERNEL_COEF, INF_LEVEL)) {

	rawValues = PrimitiveArrayProvider<float>::get(arraySize, 0.f);
	kernelValues = PrimitiveArrayProvider<float>::get(arraySize, 0.f);
	nonZeroRawIndexes.reserve(MAX_NON_ZERO_INDEXES);

	if (history) {
		minimalThreshold = HISTORY_MINIMAL_THRESHOLD;
		vanishCoef = HISTORY_VANISH_COEF;
		pendingValues = PrimitiveArrayProvider<float>::get(arraySize, 0.f);
		pendingNonZeroIndexes.reserve(MAX_PENDING_NON_ZERO_INDEXES);
	}
}

InfluenceMap::~InfluenceMap() {
	PrimitiveArrayProvider<float>::release(rawValues, arraySize);
	PrimitiveArrayProvider<float>::release(pendingValues, arraySize);
	PrimitiveArrayProvider<float>::release(kernelValues, arraySize);
	PrimitiveArrayProvider<float>::release(quadValues, quadArraySize);
}

void InfluenceMap::update(unsigned index, float value) {
	assert(value >= 0.f);
	if (value == 0.f) { return; }
	if (hasHistory()) {
		if (pendingValues[index] == 0.f && pendingNonZeroIndexes.size() < MAX_PENDING_NON_ZERO_INDEXES) {
			pendingNonZeroIndexes.push_back(index);
		}
		pendingValues[index] += value;
	} else {
		if (rawValues[index] == 0.f) {
			// make sure its added once
			addNonZeroIndex(nonZeroRawIndexes, index);
		}
		rawValues[index] += value;
		invalidateCaches();
	}
}

void InfluenceMap::update(const Urho3D::Vector2& pos, float value) {
	update(calculator->indexFromPosition(pos), value);
}

void InfluenceMap::reset() {
	invalidateCaches();
	const bool rawIndexesAtCapacity = nonZeroRawIndexes.size() >= MAX_NON_ZERO_INDEXES;
	if (hasHistory()) {
		const bool pendingIndexesAtCapacity = pendingNonZeroIndexes.size() >= MAX_PENDING_NON_ZERO_INDEXES;
		if (pendingIndexesAtCapacity || rawIndexesAtCapacity) {
			//fullscan
			nonZeroRawIndexes.clear();
			const auto rawEnd = rawValues + arraySize;
			auto* pending = pendingValues;
			for (auto raw = rawValues; raw < rawEnd; ++raw, ++pending) {
				*raw = *raw * vanishCoef + *pending;
				*pending = 0.f;

				if (*raw >= minimalThreshold) {
					addNonZeroIndex(nonZeroRawIndexes, static_cast<unsigned>(raw - rawValues));
				} else {
					*raw = 0.f;
				}
			}
		} else {
			pendingNonZeroIndexes.append_range(nonZeroRawIndexes);
			sortAndRemoveDuplicates(pendingNonZeroIndexes);
			nonZeroRawIndexes.clear();
			for (unsigned index : pendingNonZeroIndexes) {
				rawValues[index] = rawValues[index] * vanishCoef + pendingValues[index];
				pendingValues[index] = 0.f;
				if (rawValues[index] >= minimalThreshold) {
					addNonZeroIndex(nonZeroRawIndexes, index);
				} else {
					rawValues[index] = 0.f;
				}
			}
		}
		pendingNonZeroIndexes.clear();
	} else {
		if (rawIndexesAtCapacity) {//full reset
			std::fill_n(rawValues, arraySize, 0.f);
		} else {
			for (unsigned index : nonZeroRawIndexes) {
				rawValues[index] = 0;
			}
		}
		nonZeroRawIndexes.clear();
	}
}

void InfluenceMap::invalidateCaches() {
	kernelDirty = true;
	centerDirty = true;
}

float InfluenceMap::getRaw(unsigned index) const { return rawValues[index]; }

float InfluenceMap::getRaw(const Urho3D::Vector2& pos) const { return getRaw(calculator->indexFromPosition(pos)); }

float InfluenceMap::getKernel(unsigned index) const {
	ensureKernel();
	return kernelValues[index];
}

std::optional<Urho3D::Vector2> InfluenceMap::getCenter() const {
	ensureCenter();
	return center;
}

void InfluenceMap::ensureKernel() const {
	if (kernelDirty) {
		std::fill_n(kernelValues, arraySize, 0.f);
		if (nonZeroRawIndexes.size() < MAX_NON_ZERO_INDEXES) {
			for (const auto index : nonZeroRawIndexes) { applyKernel(index); }
		} else {
			for (unsigned index = 0; index < arraySize; ++index) {
				if (rawValues[index] != 0.f) { applyKernel(index); }
			}
		}
		const auto [minPtr, maxPtr] = std::minmax_element(kernelValues, kernelValues + arraySize);
		minKernel = *minPtr;
		maxKernel = *maxPtr;
		kernelDirty = false;
	}
}

void InfluenceMap::initializeQuad() const {
	if (quadValues) { return; }
	unsigned int quadArraySize = 0;
	auto currentRes = calculator->getResolution();
	while (currentRes % 2 == 0 && currentRes >= 6) {
		currentRes /= 2;
		quadArraySize += currentRes * currentRes;
	}
	this->quadArraySize = quadArraySize;
	quadValues = PrimitiveArrayProvider<float>::get(quadArraySize, 0.f);
	float* ptr = quadValues;
	for (auto i = currentRes; i < calculator->getResolution(); i *= 2) {
		const auto size = i * i;
		quadLayers.emplace_back(ptr, size);
		quadResolutions.push_back(i);
		ptr += size;
	}
	assert(!quadLayers.empty());
}

void InfluenceMap::applyKernel(unsigned index) const {
	float value = rawValues[index];
	if (value == 0.f) { return; }
	auto [centerX, centerZ] = calculator->getCords(index);
	const auto minI = calculator->getValidLow(centerX - INF_LEVEL);
	const auto maxI = calculator->getValidHigh(centerX + INF_LEVEL);
	const auto minJ = calculator->getValidLow(centerZ - INF_LEVEL);
	const auto maxJ = calculator->getValidHigh(centerZ + INF_LEVEL);
	const auto jStart = (minJ - centerZ + INF_LEVEL);
	for (auto x = minI; x <= maxI; ++x) {
		auto* t = &kernelValues[calculator->getNotSafeIndex(x, minJ)];
		auto idx = (x - centerX + INF_LEVEL) * (INF_LEVEL * 2 + 1) + jStart;
		auto ptr = templateV + idx;
		for (auto y = minJ; y <= maxJ; ++y) { *(t++) += value * *(ptr++); }
	}
}

int InfluenceMap::getMaxElement(const std::array<int, 4>& indexes, std::span<const float> vals) const {
	float values1[4] = {vals[indexes[0]], vals[indexes[1]], vals[indexes[2]], vals[indexes[3]]};
	int i = std::distance(values1, std::max_element(values1, values1 + 4));
	return indexes[i];
}

void InfluenceMap::ensureCenter() const {
	if (!centerDirty) { return; }

	rebuildQuad();

	const auto maxIt = std::ranges::max_element(quadLayers[0]);
	if (*maxIt <= 0.f) {
		center.reset();
		centerDirty = false;
		return;
	}

	int maxIdx = std::distance(quadLayers[0].begin(), maxIt);
	unsigned short res = quadResolutions[0];
	for (std::size_t i = 1; i < quadLayers.size(); ++i) {
		std::array<int, 4> indexes = getCordsInHigher(res, maxIdx);
		maxIdx = getMaxElement(indexes, quadLayers[i]);
		res *= 2;
	}
	auto rawSpan = std::span<const float>(rawValues, arraySize);
	std::array<int, 4> indexes = getCordsInHigher(res, maxIdx);
	maxIdx = getMaxElement(indexes, rawSpan);
	center = calculator->getCenter(maxIdx);
	centerDirty = false;
}

void InfluenceMap::rebuildQuad() const {
	initializeQuad();
	std::span<const float> parent(rawValues, arraySize);
	unsigned short parentRes = calculator->getResolution();
	for (int i = static_cast<int>(quadLayers.size()) - 1; i >= 0; --i) {
		auto current = quadLayers[i];
		const unsigned short currentRes = quadResolutions[i];

		for (auto prow = 0; prow < parentRes; prow += 2) {
			const int row0 = prow * parentRes;
			const int row1 = row0 + parentRes;
			int child = (prow >> 1) * currentRes;

			for (auto pcol = 0; pcol < parentRes; pcol += 2, ++child) {
				const int j = row0 + pcol;
				current[child] = parent[j] + parent[j + 1] + parent[row1 + pcol] + parent[row1 + pcol + 1];
			}
		}

		parent = std::span<const float>(current.data(), current.size());
		parentRes = currentRes;
	}
}

bool InfluenceMap::cumulateErrors(float percent, std::span<float> intersection) {
	assert(intersection.size() == arraySize);
	if (std::abs(percent) < 0.05f) { return false; }
	ensureKernel();
	const float diff = maxKernel - minKernel;
	if (diff == 0.f) { return false; }

	const auto coef1 = 1.f / diff * percent;
	if (percent < 0.f) {
		for (unsigned i = 0; i < arraySize; ++i) {
			float val = (kernelValues[i] - minKernel) * coef1;
			intersection[i] += val * val;
		}
	} else {
		for (unsigned i = 0; i < arraySize; ++i) {
			float val = (maxKernel - kernelValues[i]) * coef1;
			intersection[i] += val * val;
		}
	}
	return true;
}
