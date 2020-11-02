#pragma once
#include "simulation/env/influence/map/InfluenceMapFloat.h"
inline bool validSizes(std::array<InfluenceMapFloat*, 5> &array) {
	for (int i = 1; i < array.size(); ++i) {
		if (array[i-1]->getResolution()!=array[i]->getResolution()) {
			return false;
		}
	} 
	return true;
}
