#pragma once
#include "InfluenceMap.h"

class InfluenceMapInt : public InfluenceMap {
public:
	InfluenceMapInt(unsigned short resolution, float size, float valueThresholdDebug);
	~InfluenceMapInt();

	void update(Physical* thing, float value = 1.f) override;
	void updateInt(Physical* thing, int value = 1) override;
	void updateInt(int index, int value = 1) const;
	void reset() override;
	char getValueAt(const Urho3D::Vector2& pos) const;
	float getValueAsPercent(const Urho3D::Vector2& pos) const override;
	float getValueAsPercent(const int index) const override;
	float getValueAt(int index) const override;
	void computeMinMax() override;
	void ensureReady() override;
	int getMaxIdx();

private:
	unsigned char* values;
};
