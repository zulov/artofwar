#pragma once
#include "InfluenceMap.h"

class InfluenceMapInt : public InfluenceMap {

public:
	InfluenceMapInt(unsigned short resolution, float size, float valueThresholdDebug);
	~InfluenceMapInt();

	void update(Physical* thing, float value = 1) override;
	void reset() override;
	char getValueAt(const Urho3D::Vector2& pos) const;
	float getValueAsPercent(const Urho3D::Vector2& pos) const override;
	float getValueAsPercent(const int index) const override;
	float getValueAt(int index) const override;
	void finishCalc() override;
	int getMaxElement() override;
	int getMaxElement(std::array<int, 4> indexes) override;
private:
	unsigned char* values;
};
