#include "InfluenceMap.h"


InfluenceMap::InfluenceMap(unsigned short resolution, float size): resolution(resolution),
                                                                   arraySize(resolution * resolution), size(size),
                                                                   calculator(resolution, size) {
}

InfluenceMap::~InfluenceMap() = default;
