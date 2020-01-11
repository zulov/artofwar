#pragma once
#include "InfluenceMap.h"
#include <optional>


class InfluenceMapFloat : public InfluenceMap {
public:
	InfluenceMapFloat(unsigned short resolution, float size, float coef, char level, float valueThresholdDebug);
	virtual ~InfluenceMapFloat();

	void update(Physical* physical, float value=1) override;
	void reset() override;
	float getValueAt(int index) const override;
	std::optional<Urho3D::Vector2> getBestIndexToBuild(short id) const;
private:
	bool validIndex(int i) const;
	float* values;
	float coef;
	char level;
};
