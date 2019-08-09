#pragma once
#include "InfluenceMap.h"

class InfluenceMapInt : public InfluenceMap {

public:
	InfluenceMapInt(unsigned short resolution, float size);
	~InfluenceMapInt();

	void update(Physical* physical) override;
	void reset() override;
	char getValue(const Urho3D::Vector2& pos);
	float getValueAt(int index) override;
private:
	unsigned char* values;
};
