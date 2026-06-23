#pragma once
#include "InfluenceMap.h"

enum class VisibilityType : char;
class LevelCache;

class VisibilityMap : public InfluenceMap {
public:
	VisibilityMap(unsigned short resolution, float size, float valueThresholdDebug);
	~VisibilityMap() override;

	void update(const Urho3D::Vector2& pos, float sRadius);
	void finishAtIndex(unsigned i) const;
	void finish();
	void reset();
	char getValueAt(const Urho3D::Vector2& pos) const;
	VisibilityType getValueAt(float x, float z) const;

	float getValueAt(unsigned index) const override;
	void computeMinMax();
	int removeUnseen(float* intersection);
	float getPercent();

private:
	void ensureReady() override;

	const int influenceRes;
	const int influenceArraySize;
	VisibilityType* values;
	float* ranges;
	std::vector<int> changedIndexes;
	bool* valuesForInfluence;
	LevelCache* levelCache;
	float percent = -1.f;
	bool percentReady = false;
	bool valuesForInfluenceReady = false;
};
