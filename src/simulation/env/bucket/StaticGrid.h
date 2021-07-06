#pragma once
#include <vector>

#include "Grid.h"

class StaticGrid: public Grid {
	StaticGrid(short resolution, float size, std::vector<float> queryRadius);
	StaticGrid(const StaticGrid& rhs) = delete;
	~StaticGrid();

	void update(Unit* unit, char team) const override;
	void update(Physical* entity) const override;

	void remove(Unit* unit, char team) const override;
	void remove(Physical* entity) const override;

	void updateNew(Unit* unit, char team) const override;
	void updateNew(Physical* physical) const override;

};
