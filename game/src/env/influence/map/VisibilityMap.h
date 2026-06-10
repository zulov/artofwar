#pragma once
#include "InfluenceMap.h"

enum class VisibilityType : char;
class LevelCache;

class VisibilityMap : public InfluenceMap {
public:
	VisibilityMap(unsigned short resolution, float size, float valueThresholdDebug);
	~VisibilityMap() override;

	void update(Physical* thing, float value = 1.f) override;
	void finishAtIndex(int i) const;
	void finish();
	void updateInt(Physical* thing, int value = 1) override;
	void updateInt(int index, int value = 1) const;
	void reset() override;
	char getValueAt(const Urho3D::Vector2& pos) const;
	VisibilityType getValueAt(float x, float z) const;
	float getValueAsPercent(const Urho3D::Vector2& pos) const override;
	float getValueAsPercent(const int index) const override;
	float getValueAt(int index) const override;
	void computeMinMax() override;
	int removeUnseen(float* intersection);
	float getPercent();

private:
	void ensureReady();
	// Side length of this (full-res) visibility grid, and of the half-res influence mask
	// (valuesForInfluence) it is downsampled into. Precomputed; both are powers of two.
	const int visibilityRes;
	const int influenceRes;
	VisibilityType* values;
	float* ranges;
	std::vector<int> changedIndexes;
	bool* valuesForInfluence;
	LevelCache* levelCache;
	float percent = -1.f;
	bool percentReady = false;
	bool valuesForInfluenceReady = false;
};
