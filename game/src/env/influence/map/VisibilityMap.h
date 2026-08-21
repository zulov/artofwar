#pragma once

#include <span>
#include <vector>

#include <Urho3D/Math/Vector2.h>

#include "env/GridCalculator.h"

enum class VisibilityType : char;
class LevelCache;

class VisibilityMap {
public:
	VisibilityMap(unsigned short resolution, float size, float valueThresholdDebug);
	~VisibilityMap();

	void draw() const;
	void update(const Urho3D::Vector2& pos, float sRadius);

	void finish();
	char getValueAt(const Urho3D::Vector2& pos) const;

	char getValueAt(unsigned index) const;
	int getIndexAt(const Urho3D::Vector2& pos) const { return calculator->indexFromPosition(pos); }
	int removeUnseen(std::span<float> intersection);
	float getPercent() const;
	unsigned short getResolution() const { return calculator->getResolution(); }

private:
	void ensureUnseenIntersectionReady();
	void finishAtIndex(unsigned i) const;

	GridCalculator* calculator;
	unsigned int arraySize;
	const float valueThresholdDebug;
	const int influenceRes;
	const int influenceArraySize;
	VisibilityType* values;
	float* ranges;
	std::vector<int> changedIndexes;
	std::vector<float> unseenIntersection;
	int visibleCount = 0;
	LevelCache* levelCache;
	float percent = -1.f;
	mutable bool valuesForInfluenceReady = false;
};
