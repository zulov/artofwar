#pragma once
#include <vector>

#include "Grid.h"

class StaticGrid : public Grid {
public:
	StaticGrid(short resolution, float size, std::vector<float> queryRadius);
	StaticGrid(const StaticGrid& rhs) = delete;
	~StaticGrid() override;

	void remove(Physical* physical) const override;

	void updateStatic(Static* staticObj, bool bulkAdd) const;

	void ensureInited(int index, int centerIndex);
	const std::vector<Physical*>& get(const Urho3D::Vector3& center, float radius);

private:
	int getIndexForRadius(float radius) const;
	std::vector<float> queryRadius;
	std::vector<Bucket*> bucketsPerRadius;
	std::vector<bool*> inited;
};
