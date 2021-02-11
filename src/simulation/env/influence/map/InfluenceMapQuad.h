#pragma once
#include <vector>
#include <Urho3D/Math/Vector2.h>
#include "InfluenceMapI.h"

struct GridCalculator;
class Physical;

class InfluenceMapQuad : public InfluenceMapI {//TODO nieportzrebne
public:
	InfluenceMapQuad(int from, int to, const unsigned short size, float valueThresholdDebug);
	virtual ~InfluenceMapQuad();

	Urho3D::Vector2 getCenter();
	void update(Physical* thing, float value = 1.f) override;
	void updateInt(Physical* thing, int value = 1) override;
	void reset() override;
	void print(Urho3D::String name) override;

	std::array<int, 4> getIndexes(unsigned short resolution, int index) const;
	int getMaxElement(const std::array<int, 4>& indexes, const float* values) const;
	unsigned short getResolution() override;//TODO to chyba niepotrzebne
	Urho3D::Vector2 getCenter(int index) override;
private:
	GridCalculator* calculator;
	void ensureReady();
	std::vector<float*> maps;
	bool dataReady = false;
	int maxRes;
	int minRes;
};
