#pragma once
#include "InfluenceMap.h"

enum class VisibilityType : char;
class LevelCache;

class VisibilityMap : public InfluenceMap {
public:
	VisibilityMap(unsigned short resolution, float size, float valueThresholdDebug);
	~VisibilityMap() override;

	void update(Physical* thing, float value = 1.f) override;
	void updateInt(Physical* thing, int value = 1) override;
	void updateInt(int index, int value = 1) const;
	void reset() override;
	char getValueAt(const Urho3D::Vector2& pos) const;
	float getValueAsPercent(const Urho3D::Vector2& pos) const override;
	float getValueAsPercent(const int index) const override;
	float getValueAt(int index) const override;
	void computeMinMax() override;
	void removeUnseen(float* intersection);

private:
	void ensureReady();
	VisibilityType* values;
	bool* valuesForInfluence;
	LevelCache* levelCache;
	bool valuesForInfluenceReady = false;
};
