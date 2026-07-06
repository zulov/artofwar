#pragma once

#include <span>
#include <vector>

#include <Urho3D/Math/Vector2.h>
#include <Urho3D/Math/Vector3.h>

#include "env/GridCalculator.h"

enum class VisibilityType : char;
class LevelCache;

class VisibilityMap {
public:
	VisibilityMap(unsigned short resolution, float size, float valueThresholdDebug);
	~VisibilityMap();

	void draw(short batch, short maxParts);
	void update(const Urho3D::Vector2& pos, float sRadius);
	void finishAtIndex(unsigned i) const;
	void finish();
	void reset();
	char getValueAt(const Urho3D::Vector2& pos) const;
	VisibilityType getValueAt(float x, float z) const;

	float getValueAt(unsigned index) const;
	int removeUnseen(std::span<float> intersection);
	float getPercent();
	unsigned short getResolution() const { return calculator->getResolution(); }

private:
	void ensureReady();
	void ensureUnseenIntersectionReady();
	void invalidateCaches() const;

	GridCalculator* calculator;
	unsigned int arraySize;
	const float valueThresholdDebug;
	void drawCell(int index, short batch) const;
	Urho3D::Vector3 getVertex(const Urho3D::Vector2& center, Urho3D::Vector2 vertex) const;
	const int influenceRes;
	const int influenceArraySize;
	VisibilityType* values;
	float* ranges;
	std::vector<int> changedIndexes;
	bool* valuesForInfluence;
	std::vector<float> unseenIntersection;
	int visibleCount = 0;
	LevelCache* levelCache;
	float percent = -1.f;
	mutable bool percentReady = false;
	mutable bool valuesForInfluenceReady = false;
	mutable bool unseenIntersectionReady = false;
};
