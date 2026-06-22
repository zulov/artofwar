#pragma once
#include "InfluenceMap.h"

class InfluenceMapInt : public InfluenceMap {
public:
	InfluenceMapInt(unsigned short resolution, float size, float valueThresholdDebug);
	~InfluenceMapInt() override;

	void update(unsigned index, unsigned char value = 1) const;
	void reset();
	unsigned char getValueAt(const Urho3D::Vector2& pos) const;
	float getValueAsPercent(unsigned index) const override;
	float getValueAt(unsigned index) const override;
	void ensureReady() override;
	std::vector<unsigned> getMaxIdxs();

private:
	unsigned char* values;
};
