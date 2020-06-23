#pragma once
#include "InfluenceMap.h"

class InfluenceMapInt : public InfluenceMap {

public:
	InfluenceMapInt(unsigned short resolution, float size, float valueThresholdDebug);
	~InfluenceMapInt();

	void update(Urho3D::Vector3& pos, float value = 1) override;
	void reset() override;
	char getValueAt(const Urho3D::Vector2& pos) const;
	float getValueAsPercent(const Urho3D::Vector2& pos) const override;
	float getValueAt(int index) const override;
	void calcStats() override;
private:
	unsigned char* values;
};
