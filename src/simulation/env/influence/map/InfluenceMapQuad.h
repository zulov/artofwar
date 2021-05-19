#pragma once
#include <optional>
#include <span>
#include <vector>
#include <Urho3D/Math/Vector2.h>
#include "InfluenceMapI.h"

struct GridCalculator;
class Physical;

class InfluenceMapQuad {
public:
	InfluenceMapQuad(unsigned short topResolution, float mapSize);
	virtual ~InfluenceMapQuad();

	std::optional<Urho3D::Vector2> getCenter();
	void update(Physical* thing, float value = 1.f);
	void update(int index, float value = 1.f);
	void updateInt(Physical* thing, int value = 1);
	void reset();
	void print(const Urho3D::String& name, std::span<float> map);
	void print(Urho3D::String name);

	unsigned short getResolution() const; //TODO to chyba niepotrzebne
private:
	std::array<int, 4> getIndexes(unsigned short resolution, int index) const;
	int getMaxElement(const std::array<int, 4>& indexes, std::span<float> values) const;
	GridCalculator* calculator;
	void ensureReady();
	std::vector<std::span<float>> maps;
	float* data;
	int dataSize;
	bool dataReady = false;//TODO bug czy dobrze jest resetowana
	unsigned short counter = 0;
};
