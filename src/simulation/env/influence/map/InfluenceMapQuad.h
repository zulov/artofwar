#pragma once
#include <vector>

#include "InfluenceMap.h"

class InfluenceMapQuad : public InfluenceMapI {
public:
	explicit InfluenceMapQuad(std::initializer_list<InfluenceMapI*> maps);
	~InfluenceMapQuad();

	Urho3D::Vector2 getCenter();
	void update(Physical* thing, float value = 1) override;
	void reset() override;
	void finishCalc() override;
	void print(Urho3D::String name) override;

	std::array<int, 4> getIndexes(unsigned short resolution, int index);
	
	int getMaxElement() override;
	int getMaxElement(std::array<int, 4> indexes) override;
	unsigned short getResolution() override;
	Urho3D::Vector2 getCenter(int index) override;
private:
	std::vector<InfluenceMapI*> maps;
};
