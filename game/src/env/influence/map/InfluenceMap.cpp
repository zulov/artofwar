#include "InfluenceMap.h"

#include <algorithm>
#include <cassert>
#include <ranges>
#include <numeric>

#include "env/influence/MapsUtils.h"
#include "env/influence/map/InfluenceTemplateProvider.h"

namespace {
	constexpr float KERNEL_COEF = 0.5f;
	constexpr float HISTORY_MINIMAL_THRESHOLD = 0.0001f;
	constexpr float HISTORY_VANISH_COEF = 0.5f;
}

InfluenceMap::InfluenceMap(GridCalculator* calculator, float valueThresholdDebug, bool history)
	: templateV(InfluenceTemplateProvider::get(KERNEL_COEF, INF_LEVEL)),
	  calculator(calculator),
	  arraySize(0),
	  valueThresholdDebug(valueThresholdDebug) {
	assert(calculator);
	arraySize = calculator->getResolution() * calculator->getResolution();
	rawValues = new float[arraySize];
	kernelValues = new float[arraySize];
	std::fill_n(rawValues, arraySize, 0.f);
	std::fill_n(kernelValues, arraySize, 0.f);

	centerDirty = true;
	if (history) {
		minimalThreshold = HISTORY_MINIMAL_THRESHOLD;
		vanishCoef = HISTORY_VANISH_COEF;
		pendingValues = new float[arraySize];
		std::fill_n(pendingValues, arraySize, 0.f);
	}
}

InfluenceMap::~InfluenceMap() {
	delete[] rawValues;
	delete[] pendingValues;
	delete[] kernelValues;
}

void InfluenceMap::update(unsigned index, float value) {
	if (hasPendingValues()) {
		pendingValues[index] += value;
	} else {
		rawValues[index] += value;
	}
	valuesCalculateNeeded = true;
	centerDirty = true;
	minMaxInited = false;
}

void InfluenceMap::update(const Urho3D::Vector2& pos, float value) {
	update(calculator->indexFromPosition(pos), value);
}

void InfluenceMap::updateFromTemp() { ensureKernel(); }

void InfluenceMap::reset() {
	if (hasPendingValues()) {
		const auto rawEnd = rawValues + arraySize;
		auto* pending = pendingValues;
		for (auto raw = rawValues; raw < rawEnd; ++raw, ++pending) {
			*raw = *raw * vanishCoef + *pending;
			*pending = 0.f;
		}
		invalidateCaches();
		return;
	}
	if (vanishCoef != 1.f || minimalThreshold != 0.f) {
		const auto end = rawValues + arraySize;
		for (auto i = rawValues; i < end; ++i) {
			*i *= vanishCoef;
		}
		invalidateCaches();
		return;
	}
	std::fill_n(rawValues, arraySize, 0.f);
	if (pendingValues) {
		std::fill_n(pendingValues, arraySize, 0.f);
	}
	std::fill_n(kernelValues, arraySize, 0.f);
	valuesCalculateNeeded = false;
	center.reset();
	centerDirty = false;
	minMaxInited = false;
}

void InfluenceMap::resetToZero() {
	const auto end = rawValues + arraySize;
	for (auto i = rawValues; i < end; ++i) {
		*i = *i >= minimalThreshold ? *i : 0.f;
	}
	invalidateCaches();
}

void InfluenceMap::invalidateCaches() {
	valuesCalculateNeeded = true;
	centerDirty = true;
	minMaxInited = false;
}

float InfluenceMap::getRaw(unsigned index) const {
	return rawValues[index];
}

float InfluenceMap::getRaw(const Urho3D::Vector2& pos) const {
	return getRaw(calculator->indexFromPosition(pos));
}

float InfluenceMap::getKernel(unsigned index) const {
	ensureKernel();
	return kernelValues[index];
}

float InfluenceMap::getKernel(const Urho3D::Vector2& pos) const {
	return getKernel(calculator->indexFromPosition(pos));
}

std::vector<unsigned> InfluenceMap::getRawMaxIdxs() const {
	return getMaxIdxsRaw();
}

std::vector<unsigned> InfluenceMap::getKernelMaxIdxs() const {
	ensureKernel();
	return getMaxIdxsKernel();
}

std::vector<unsigned> InfluenceMap::getMaxIdxsRaw() const {
	const auto data = std::span<const float>(rawValues, arraySize);
	if (data.empty()) {
		return {};
	}
	const auto maxIt = std::ranges::max_element(data);
	if (maxIt == data.end() || *maxIt <= 0.5f) {
		return {};
	}
	std::vector<unsigned> idx(arraySize);
	std::iota(idx.begin(), idx.end(), 0u);
	std::sort(idx.begin(), idx.end(), [data](unsigned a, unsigned b) {
		return data[a] > data[b];
	});
	idx.resize(std::min<std::size_t>(10, idx.size()));
	while (!idx.empty() && data[idx.back()] == 0) {
		idx.pop_back();
	}
	return idx;
}

std::vector<unsigned> InfluenceMap::getMaxIdxsKernel() const {
	const auto data = std::span<const float>(kernelValues, arraySize);
	if (data.empty()) {
		return {};
	}
	const auto maxIt = std::ranges::max_element(data);
	if (maxIt == data.end() || *maxIt <= 0.5f) {
		return {};
	}
	std::vector<unsigned> idx(arraySize);
	std::iota(idx.begin(), idx.end(), 0u);
	std::sort(idx.begin(), idx.end(), [data](unsigned a, unsigned b) {
		return data[a] > data[b];
	});
	idx.resize(std::min<std::size_t>(10, idx.size()));
	while (!idx.empty() && data[idx.back()] == 0) {
		idx.pop_back();
	}
	return idx;
}

void InfluenceMap::ensureKernel() const {
	if (valuesCalculateNeeded) {
		rebuildKernel();
	}
}

void InfluenceMap::rebuildKernel() const {
	std::fill_n(kernelValues, arraySize, 0.f);
	for (unsigned i = 0; i < arraySize; ++i) {
		const auto value = rawValues[i];
		if (value == 0.f) {
			continue;
		}
		auto [centerX, centerZ] = calculator->getCords(i);
		const auto minI = calculator->getValidLow(centerX - INF_LEVEL);
		const auto maxI = calculator->getValidHigh(centerX + INF_LEVEL);
		const auto minJ = calculator->getValidLow(centerZ - INF_LEVEL);
		const auto maxJ = calculator->getValidHigh(centerZ + INF_LEVEL);
		const auto jStart = (minJ - centerZ + INF_LEVEL);
		for (auto x = minI; x <= maxI; ++x) {
			auto* t = &kernelValues[calculator->getNotSafeIndex(x, minJ)];
			auto idx = (x - centerX + INF_LEVEL) * (INF_LEVEL * 2 + 1) + jStart;
			auto ptr = templateV + idx;
			for (short y = minJ; y <= maxJ; ++y) {
				*(t++) += value * *(ptr++);
			}
		}
	}
	valuesCalculateNeeded = false;
}

void InfluenceMap::ensureReady() {
	ensureKernel();
	computeMinMax();
}

void InfluenceMap::computeMinMax() const {
	if (!minMaxInited) {
		ensureKernel();
		const auto [minPtr, maxPtr] = std::minmax_element(kernelValues, kernelValues + arraySize);
		min = *minPtr;
		max = *maxPtr;
		minMaxInited = true;
	}
}

std::vector<int> InfluenceMap::getIndexesWithByValue(float percent, float tolerance) {
	ensureKernel();
	computeMinMax();
	const float diff = max - min;
	auto minV = diff * (percent - tolerance) + min;
	auto maxV = diff * (percent + tolerance) + min;
	float* i = kernelValues;
	std::vector<int> indexes;
	indexes.reserve(16);
	auto pred = [minV, maxV](float v) { return v >= minV && v <= maxV; };
	while ((i = std::find_if(i, kernelValues + arraySize, pred)) != kernelValues + arraySize) {
		indexes.push_back(i - kernelValues);
		++i;
	}
	return indexes;
}

bool InfluenceMap::cumulateErrors(float percent, std::span<float> intersection) {
	ensureKernel();
	computeMinMax();
	assert(minMaxInited);
	const float diff = max - min;
	if (diff == 0.f) {
		return false;
	}
	assert(intersection.size() == arraySize);
	const auto coef1 = 1.f / diff * percent;
	if (percent < 0.f) {
		for (unsigned i = 0; i < arraySize; ++i) {
			float val = (kernelValues[i] - min) * coef1;
			intersection[i] += val * val;
		}
	} else {
		for (unsigned i = 0; i < arraySize; ++i) {
			float val = (max - kernelValues[i]) * coef1;
			intersection[i] += val * val;
		}
	}
	return true;
}
