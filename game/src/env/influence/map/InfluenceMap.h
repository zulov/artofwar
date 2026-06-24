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
	InfluenceMap(unsigned short resolution, float size, float coef, char level, float valueThresholdDebug,
	             float* sharedTemplateV, bool ownsTemplateV = false);
	InfluenceMap(unsigned short resolution, float size, float coef, char level, float minimalThreshold,
	             float vanishCoef, float valueThresholdDebug, float* sharedTemplateV, bool ownsTemplateV = false);
	InfluenceMap(unsigned short resolution, float size, float valueThresholdDebug);
	InfluenceMap(unsigned short resolution, float size);
	virtual ~InfluenceMap();

	void drawRaw(short batch, short maxParts);
	void drawKernel(short batch, short maxParts);

	void update(unsigned index, float value = 1.f);
	void update(const Urho3D::Vector2& pos, float value = 1.f);
	void updateInt(unsigned index, unsigned char value = 1);
	void updateFromTemp();

	void reset();
	void resetToZero();

	float getRaw(unsigned index) const;
	float getRaw(const Urho3D::Vector2& pos) const;
	float getKernel(unsigned index) const;
	float getKernel(const Urho3D::Vector2& pos) const;
	std::optional<Urho3D::Vector2> getCenter() const;
	std::vector<unsigned> getRawMaxIdxs() const;
	std::vector<unsigned> getKernelMaxIdxs() const;
	void print(Urho3D::String name);
	unsigned short getResolution() const { return calculator->getResolution(); }

	std::vector<int> getIndexesWithByValue(float percent, float tolerance);
	bool cumulateErrors(float percent, float* intersection);

	void ensureReady();

	static float* createTemplateV(float coef, char level);

protected:
	GridCalculator* calculator;
	unsigned int arraySize;
	const float valueThresholdDebug;

	float* rawValues;
	float* values;
	float coef;
	mutable bool valuesCalculateNeeded = false;
	void invalidateCaches();
	void printMap(std::span<const float> map, const Urho3D::String& name);

	mutable float min = 0.f;
	mutable float max = 0.f;

	unsigned short counter = 0;
	mutable bool minMaxInited = false;

private:
	std::vector<unsigned> getMaxIdxsRaw() const;
	std::vector<unsigned> getMaxIdxsKernel() const;
	void ensureKernel() const;
	void ensureQuad() const;
	void rebuildKernel() const;
	void rebuildQuad() const;
	int getMaxElement(const std::array<int, 4>& indexes, std::span<float> vals) const;
	void computeMinMax() const;
	Urho3D::Vector3 getVertex(const Urho3D::Vector2& center, Urho3D::Vector2 vertex) const;
	void drawCell(int index, short batch, bool useKernel) const;

	unsigned char level;
	unsigned char levelRes;
	float* templateV;
	bool ownsTemplateV = false;
	float* quadValues;
	unsigned int quadArraySize;
	std::vector<std::span<float>> quadLayers;
	std::vector<unsigned short> quadResolutions;
	mutable bool quadDirty = true;
	float minimalThreshold = 0.f;
	float vanishCoef = 1.f;
};
