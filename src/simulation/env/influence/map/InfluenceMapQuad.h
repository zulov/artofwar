#pragma once
#include <vector>
#include <Urho3D/Math/Vector2.h>
#include "InfluenceMapI.h"
#include "simulation/env/influence/InfluenceMapType.h"

struct GridCalculator;
class Physical;

class InfluenceMapQuad : public InfluenceMapI {
public:
	InfluenceMapQuad(int from, int to, InfluenceMapType type, const unsigned short size, float coef,
	                 float valueThresholdDebug);
	virtual ~InfluenceMapQuad();

	Urho3D::Vector2 getCenter();
	void update(Physical* thing, float value = 1.f) override;
	void updateInt(Physical* thing, int value = 1) override;
	void reset() override;
	void print(Urho3D::String name) override;

	std::array<int, 4> getIndexes(unsigned short resolution, int index) const;
	int getMaxElement(const std::array<int, 4>& indexes, const float* values) const;
private:
	GridCalculator* calculator;
	void ensureReady();
	std::vector<float*> maps;
	bool dataReady = false;
	int maxSize = false;
	int minSize = false;
};
