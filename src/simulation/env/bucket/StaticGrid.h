#pragma once
#include <vector>

#include "Grid.h"

class StaticGrid: public Grid {
public:
	StaticGrid(short resolution, float size, std::vector<float> queryRadius);
	StaticGrid(const StaticGrid& rhs) = delete;
	~StaticGrid() override;

	void update(Unit* unit, char team) const override;
	void remove(Unit* unit, char team) const override;
	void updateNew(Unit* unit, char team) const override;
	void update(Physical* physical) const override;

	void remove(Physical* physical) const override;
	void updateNew(Physical* physical) const override;
	
	const std::vector<Physical*>& get(const Urho3D::Vector3& center, int id, float radius, float prevRadius);
private:
	std::vector<float> queryRadius;
	std::vector<Bucket*> bucketsPerRadius;
};
