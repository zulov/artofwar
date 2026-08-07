#pragma once

#include <array>
#include <optional>
#include <span>
#include <vector>

#include <Urho3D/Container/Str.h>
#include <Urho3D/Math/Vector2.h>

#include "env/GridCalculator.h"

class InfluenceMap {
public:
	InfluenceMap(GridCalculator* calculator, float valueThresholdDebug = 40.f, bool history = false);
	virtual ~InfluenceMap();
	InfluenceMap(const InfluenceMap&) = delete;
	InfluenceMap& operator=(const InfluenceMap&) = delete;
	InfluenceMap(InfluenceMap&&) = delete;
	InfluenceMap& operator=(InfluenceMap&&) = delete;

	void drawRaw();
	void drawKernel();

	void update(unsigned index, float value = 1.f);
	void update(const Urho3D::Vector2& pos, float value = 1.f);

	void reset();
	void resetToZero();

	float getRaw(unsigned index) const;
	float getRaw(const Urho3D::Vector2& pos) const;
	float getKernel(unsigned index) const;
	float getKernel(const Urho3D::Vector2& pos) const;
	std::optional<Urho3D::Vector2> getCenter() const;
	std::vector<unsigned> getRawMaxIdxs() const;
	void print(Urho3D::String name);
	unsigned short getResolution() const { return calculator->getResolution(); }

	bool cumulateErrors(float percent, std::span<float> intersection);

protected:
	GridCalculator* calculator;
	unsigned int arraySize;
	const float valueThresholdDebug;

	float* rawValues;
	float* pendingValues = nullptr;
	float* kernelValues;
	mutable bool valuesCalculateNeeded = false;
	std::vector<unsigned> nonZeroIndexes;
	void invalidateCaches();
	void printMap(std::span<const float> map, const Urho3D::String& name);

	mutable float min = 0.f;
	mutable float max = 0.f;

	unsigned short counter = 0;
	mutable bool minMaxInited = false;
	bool hasPendingValues() const { return pendingValues != nullptr; }

private:
	void applyKernel(unsigned index, float value) const;
	std::vector<unsigned> getMaxIdxs(std::span<const float> values) const;
	void ensureCenter() const;
	void ensureKernel() const;
	void initializeQuad() const;
	void rebuildKernel() const;
	void rebuildQuad() const;
	int getMaxElement(const std::array<int, 4>& indexes, std::span<const float> vals) const;
	void computeMinMax() const;

	const float* templateV;
	mutable float* quadValues = nullptr;
	mutable std::vector<std::span<float>> quadLayers;
	mutable std::vector<unsigned short> quadResolutions;
	mutable std::optional<Urho3D::Vector2> center;
	mutable bool centerDirty = true;
	float minimalThreshold = 0.f;
	float vanishCoef = 1.f;
};
