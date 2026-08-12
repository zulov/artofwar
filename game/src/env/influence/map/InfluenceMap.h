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

	float getRaw(unsigned index) const;
	float getRaw(const Urho3D::Vector2& pos) const;
	float getKernel(unsigned index) const;
	std::optional<Urho3D::Vector2> getCenter() const;
	std::vector<unsigned> getRawMaxIdxs() const;
	void print(Urho3D::String name);
	unsigned short getResolution() const { return calculator->getResolution(); }

	bool cumulateErrors(float percent, std::span<float> intersection);
	void ensureKernel() const;

protected:
	unsigned int arraySize;

	mutable float minKernel = 0.f;
	mutable float maxKernel = 0.f;
	std::vector<unsigned> nonZeroRawIndexes;
	std::vector<unsigned> pendingNonZeroIndexes;
	float* kernelValues{};

	mutable bool kernelDirty = false;
	mutable bool centerDirty = true;

private:
	void invalidateCaches();
	void printMap(std::span<const float> map, const Urho3D::String& name);
	bool hasHistory() const { return pendingValues != nullptr; }
	void applyKernel(unsigned index) const;
	std::vector<unsigned> getMaxIdxs(std::span<const float> values) const;
	void ensureCenter() const;

	void initializeQuad() const;
	void rebuildQuad() const;
	int getMaxElement(const std::array<int, 4>& indexes, std::span<const float> vals) const;

	GridCalculator* calculator;

	const float debugThreshold;

	float* rawValues{};//TODO Ai witch is better decay raw or kernal
	float* pendingValues{};

	const float* templateV;
	mutable float* quadValues = nullptr;

	mutable std::vector<std::span<float>> quadLayers;
	mutable std::vector<unsigned short> quadResolutions;
	mutable std::optional<Urho3D::Vector2> center;

	float minimalThreshold = 0.f;
	float vanishCoef = 1.f;
	unsigned short counter = 0;
};
