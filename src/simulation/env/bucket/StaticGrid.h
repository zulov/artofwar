#pragma once
#include <vector>

#include "Grid.h"
#include "objects/resource/ResourceEntity.h"

class StaticGrid : public Grid {
public:
	StaticGrid(short resolution, float size, std::vector<float> queryRadius);
	StaticGrid(const StaticGrid& rhs) = delete;
	~StaticGrid() override;

	//void updateSparse(Unit* unit, char team) const override;
	//void remove(Unit* unit, char team) const override;
	//int updateNew(Physical* unit) const override;
	int update(Physical* physical, int currentIndex) const override;
	void remove(Physical* physical) const;

	//void remove(Physical* physical) const override;
	void updateStatic(Static* staticObj, bool bulkAdd) const;
	
	void initAdd() const;

	const std::vector<Physical*>& get(const Urho3D::Vector3& center, float radius);

private:
	int getIndexForRadius(float radius) const;
	std::vector<float> queryRadius;
	std::vector<Bucket*> bucketsPerRadius;
};
