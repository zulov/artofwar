#pragma once
#include <vector>
#include <Urho3D/Math/Vector2.h>
#include "InfluenceMapI.h"
#include "simulation/env/influence/InfluenceMapType.h"

class Physical;

class InfluenceMapQuad : public InfluenceMapI {
public:
	InfluenceMapQuad(int from, int to, InfluenceMapType type, const unsigned short size, float coef, char level,
	                 float valueThresholdDebug);
	virtual ~InfluenceMapQuad();

	Urho3D::Vector2 getCenter();
	void update(Physical* thing, float value = 1.f) override;
	void reset() override;
	void finishCalc() override;
	void print(Urho3D::String name) override;

	std::array<int, 4> getIndexes(unsigned short resolution, int index) const;

	int getMaxElement() override;
	int getMaxElement(const std::array<int, 4>& indexes) override;
	unsigned short getResolution() override;
	Urho3D::Vector2 getCenter(int index) override;
private:
	std::vector<InfluenceMapI*> maps;
};
