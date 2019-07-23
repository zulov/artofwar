#pragma once
#include "InfluenceMap.h"

class InfluenceMapInt : InfluenceMap {

public:
	InfluenceMapInt(unsigned short resolution, float size);
	~InfluenceMapInt();

	void update(Physical* physical) override;
	void reset() override;
	char getValue(const Urho3D::Vector2& pos);
private:
	unsigned char* values;
};
