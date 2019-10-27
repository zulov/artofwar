#pragma once
#include "InfluenceMap.h"

class InfluenceMapInt : public InfluenceMap {

public:
	InfluenceMapInt(unsigned short resolution, float size, float valueThresholdDebug);
	~InfluenceMapInt();

	void update(Physical* physical, float value = 1) override;
	void reset() override;
	char getValue(const Urho3D::Vector2& pos);
	float getValueAt(int index) const override;
private:
	unsigned char* values;
};
