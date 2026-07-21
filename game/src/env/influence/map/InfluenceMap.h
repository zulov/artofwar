#pragma once

#include <array>
#include <optional>
#include <span>
#include <vector>

#include <Urho3D/Container/Str.h>
#include <Urho3D/Math/Vector2.h>
#include <Urho3D/Math/Vector3.h>

#include "env/GridCalculator.h"

class InfluenceMap {
public:
	InfluenceMap(GridCalculator* calculator, float valueThresholdDebug = 40.f, bool history = false);
	virtual ~InfluenceMap();

	void drawRaw(short batch, short maxParts);
	void drawRaw(std::span<const float> values, short batch, short maxParts);
	void drawKernel(short batch, short maxParts);

	void update(unsigned index, float value = 1.f);
	void update(const Urho3D::Vector2& pos, float value = 1.f);
	void updateFromTemp();

	void reset();
	void resetToZero();

	float getRaw(unsigned index) const;
	float getRaw(const Urho3D::Vector2& pos) const;
	float getKernel(unsigned index) const;
	float getKernel(const Urho3D::Vector2& pos) const;
	std::vector<unsigned> getRawMaxIdxs() const;
	std::vector<unsigned> getKernelMaxIdxs() const;
	void print(Urho3D::String name);
	unsigned short getResolution() const { return calculator->getResolution(); }

	std::vector<int> getIndexesWithByValue(float percent, float tolerance);
	bool cumulateErrors(float percent, std::span<float> intersection);

	void ensureReady();

protected:
	GridCalculator* calculator;
	unsigned int arraySize;
	const float valueThresholdDebug;

	float* rawValues;
	float* pendingValues = nullptr;
	float* kernelValues;
	mutable bool valuesCalculateNeeded = false;
	void invalidateCaches();
	void printMap(std::span<const float> map, const Urho3D::String& name);

	mutable float min = 0.f;
	mutable float max = 0.f;

	unsigned short counter = 0;
	mutable bool minMaxInited = false;
	bool hasPendingValues() const { return pendingValues != nullptr; }

private:
	friend class InfluenceManager;

	std::vector<unsigned> getMaxIdxsRaw() const;
	std::vector<unsigned> getMaxIdxsKernel() const;
	void ensureKernel() const;
	void rebuildKernel() const;
	void computeMinMax() const;
	Urho3D::Vector3 getVertex(const Urho3D::Vector2& center, Urho3D::Vector2 vertex) const;
	void drawCell(int index, short batch, float value) const;

	const float* templateV;
	mutable std::optional<Urho3D::Vector2> center;
	mutable bool centerDirty = true;
	float minimalThreshold = 0.f;
	float vanishCoef = 1.f;
};
