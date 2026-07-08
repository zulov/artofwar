#include "InfluenceMap.h"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <ranges>
#include <numeric>

#include "env/GridCalculatorProvider.h"
#include "math/MathUtils.h"
#include "utils/SpanUtils.h"

namespace {
enum class QuadRebuildVariant {
	A,
	B,
};

// Flip this to B to benchmark the block-based reduction.
constexpr QuadRebuildVariant kQuadRebuildVariant = QuadRebuildVariant::A;
}

InfluenceMap::InfluenceMap(unsigned short resolution, float size, float coef, char level,
	                         float valueThresholdDebug, float* sharedTemplateV, bool ownsTemplateV)
	: coef(coef),
	  level(level),
	  templateV(sharedTemplateV),
	  ownsTemplateV(ownsTemplateV),
	  calculator(GridCalculatorProvider::get(resolution, size)),
	  arraySize(resolution * resolution),
	  valueThresholdDebug(valueThresholdDebug) {
	assert(level > 0);
	levelRes = level * 2 + 1;

	rawValues = new float[arraySize];
	values = new float[arraySize];
	std::fill_n(rawValues, arraySize, 0.f);
	std::fill_n(values, arraySize, 0.f);

	quadArraySize = 0;
	auto currentRes = resolution;
	while (currentRes % 2 == 0 && currentRes != 1) {
		currentRes /= 2;
	}
	currentRes *= 2;
	for (int i = currentRes; i < resolution; i *= 2) {
		quadArraySize += i * i;
	}
	quadValues = new float[quadArraySize];
	std::fill_n(quadValues, quadArraySize, 0.f);
	float* ptr = quadValues;
	for (int i = currentRes; i < resolution; i *= 2) {
		const auto size1 = i * i;
		quadLayers.emplace_back(ptr, size1);
		quadResolutions.push_back(i);
		ptr += size1;
	}
	assert(!quadLayers.empty());
	centerDirty = true;
}

InfluenceMap::InfluenceMap(unsigned short resolution, float size, float coef, char level, float minimalThreshold,
	                         float vanishCoef, float valueThresholdDebug, float* sharedTemplateV, bool ownsTemplateV)
	: InfluenceMap(resolution, size, coef, level, valueThresholdDebug, sharedTemplateV, ownsTemplateV) {
	this->minimalThreshold = minimalThreshold;
	this->vanishCoef = vanishCoef;
}

InfluenceMap::InfluenceMap(unsigned short resolution, float size, float valueThresholdDebug)
	: InfluenceMap(resolution, size, 0.5f, 4, valueThresholdDebug, createTemplateV(0.5f, 4), true) {}

InfluenceMap::InfluenceMap(unsigned short resolution, float size)
	: InfluenceMap(resolution, size, 0.5f, 4, 0.f, createTemplateV(0.5f, 4), true) {}

InfluenceMap::~InfluenceMap() {
	delete[] rawValues;
	delete[] values;
	delete[] quadValues;
	if (ownsTemplateV) {
		delete[] templateV;
	}
}

float* InfluenceMap::createTemplateV(float coef, char level) {
	const auto levelRes = level * 2 + 1;
	auto* tv = new float[levelRes * levelRes];
	auto* ptr = tv;
	for (short i = -level; i <= level; ++i) {
		const auto a = i * i;
		for (short j = -level; j <= level; ++j) {
			const auto b = j * j;
			*(ptr++) = 1 / ((a + b) * coef + 1.f);
		}
	}
	return tv;
}

void InfluenceMap::update(unsigned index, float value) {
	rawValues[index] += value;
	valuesCalculateNeeded = true;
	centerDirty = true;
	quadDirty = true;
	minMaxInited = false;
}

void InfluenceMap::update(const Urho3D::Vector2& pos, float value) {
	update(calculator->indexFromPosition(pos), value);
}

void InfluenceMap::updateFromTemp() { ensureKernel(); }

void InfluenceMap::reset() {
	if (vanishCoef != 1.f || minimalThreshold != 0.f) {
		const auto end = rawValues + arraySize;
		for (auto i = rawValues; i < end; ++i) {
			*i *= vanishCoef;
		}
		invalidateCaches();
		return;
	}
	std::fill_n(rawValues, arraySize, 0.f);
	std::fill_n(values, arraySize, 0.f);
	std::fill_n(quadValues, quadArraySize, 0.f);
	valuesCalculateNeeded = false;
	center.reset();
	centerDirty = false;
	quadDirty = true;
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
	quadDirty = true;
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
	return values[index];
}

float InfluenceMap::getKernel(const Urho3D::Vector2& pos) const {
	return getKernel(calculator->indexFromPosition(pos));
}

std::optional<Urho3D::Vector2> InfluenceMap::getCenter() const {
	ensureCenter();
	return center;
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
	const auto data = std::span<const float>(values, arraySize);
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

void InfluenceMap::ensureQuad() const {
	if (quadDirty) {
		rebuildQuad();
	}
}

void InfluenceMap::rebuildKernel() const {
	std::fill_n(values, arraySize, 0.f);
	for (unsigned i = 0; i < arraySize; ++i) {
		const auto value = rawValues[i];
		if (value == 0.f) {
			continue;
		}
		auto [centerX, centerZ] = calculator->getCords(i);
		const auto minI = calculator->getValidLow(centerX - level);
		const auto maxI = calculator->getValidHigh(centerX + level);
		const auto minJ = calculator->getValidLow(centerZ - level);
		const auto maxJ = calculator->getValidHigh(centerZ + level);
		const auto jStart = (minJ - centerZ + level);
		for (auto x = minI; x <= maxI; ++x) {
			auto* t = &values[calculator->getNotSafeIndex(x, minJ)];
			auto idx = (x - centerX + level) * levelRes + jStart;
			auto ptr = templateV + idx;
			for (short y = minJ; y <= maxJ; ++y) {
				*(t++) += value * *(ptr++);
			}
		}
	}
	valuesCalculateNeeded = false;
}

int InfluenceMap::getMaxElement(const std::array<int, 4>& indexes, std::span<const float> vals) const {
	float values1[4] = {vals[indexes[0]], vals[indexes[1]], vals[indexes[2]], vals[indexes[3]]};
	int i = std::distance(values1, std::max_element(values1, values1 + 4));
	return indexes[i];
}

void InfluenceMap::ensureCenter() const {
	if (!centerDirty) {
		return;
	}

	ensureQuad();
	auto rawSpan = std::span<const float>(rawValues, arraySize);

	if (!anyGreaterThanZero(quadLayers[0])) {
		center.reset();
		centerDirty = false;
		return;
	}

	int index = std::distance(quadLayers[0].begin(), std::ranges::max_element(quadLayers[0]));
	unsigned short res = quadResolutions[0];
	for (int i = 1; i < static_cast<int>(quadLayers.size()); ++i) {
		std::array<int, 4> indexes = getCordsInHigher(res, index);
		index = getMaxElement(indexes, quadLayers[i]);
		res *= 2;
	}
	std::array<int, 4> indexes = getCordsInHigher(res, index);
	index = getMaxElement(indexes, rawSpan);
	center = calculator->getCenter(index);
	centerDirty = false;
}

void InfluenceMap::rebuildQuad() const {
	std::fill_n(quadValues, quadArraySize, 0.f);
	if constexpr (kQuadRebuildVariant == QuadRebuildVariant::A) {
		rebuildQuadVersionA();
	} else {
		rebuildQuadVersionB();
	}
	quadDirty = false;
}


void InfluenceMap::rebuildQuadVersionA() const {
	std::span<const float> parent(rawValues, arraySize);
	unsigned short parentRes = calculator->getResolution();
	for (int i = static_cast<int>(quadLayers.size()) - 1; i >= 0; --i) {
		auto current = quadLayers[i];
		const unsigned short currentRes = quadResolutions[i];
		assert(parentRes == currentRes * 2);

		const int parentSize = static_cast<int>(parent.size());
		for (int j = 0; j < parentSize; j += 2) {
			const float sum = parent[j] + parent[j + 1];
			if (sum > 0.f) {
				const int newIndex = getCordsInLower(currentRes, parentRes, j);
				assert(newIndex < currentRes * currentRes);
				current[newIndex] += sum;
			}
		}

		parent = std::span<const float>(current.data(), current.size());
		parentRes = currentRes;
	}
}

void InfluenceMap::rebuildQuadVersionB() const {
	std::span<const float> parent(rawValues, arraySize);
	unsigned short parentRes = calculator->getResolution();
	for (int i = static_cast<int>(quadLayers.size()) - 1; i >= 0; --i) {
		auto current = quadLayers[i];
		const unsigned short currentRes = quadResolutions[i];
		assert(parentRes == currentRes * 2);

		for (int prow = 0; prow < parentRes; prow += 2) {
			const int row0 = prow * parentRes;
			const int row1 = row0 + parentRes;
			int child = (prow >> 1) * currentRes;

			for (int pcol = 0; pcol < parentRes; pcol += 2, ++child) {
				const int j = row0 + pcol;
				current[child] = parent[j] + parent[j + 1] + parent[row1 + pcol] + parent[row1 + pcol + 1];
			}
		}

		parent = std::span<const float>(current.data(), current.size());
		parentRes = currentRes;
	}
}

void InfluenceMap::ensureReady() {
	ensureKernel();
	ensureQuad();
	computeMinMax();
}

void InfluenceMap::computeMinMax() const {
	if (!minMaxInited) {
		ensureKernel();
		const auto [minPtr, maxPtr] = std::minmax_element(values, values + arraySize);
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
	float* i = values;
	std::vector<int> indexes;
	indexes.reserve(16);
	auto pred = [minV, maxV](float v) { return v >= minV && v <= maxV; };
	while ((i = std::find_if(i, values + arraySize, pred)) != values + arraySize) {
		indexes.push_back(i - values);
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
			float val = (values[i] - min) * coef1;
			intersection[i] += val * val;
		}
	} else {
		for (unsigned i = 0; i < arraySize; ++i) {
			float val = (max - values[i]) * coef1;
			intersection[i] += val * val;
		}
	}
	return true;
}
