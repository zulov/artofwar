#pragma once
#include "env/influence/map/InfluenceMapFloat.h"
inline bool validSizes(std::array<InfluenceMapFloat*, 8> &array) {
	for (int i = 1; i < array.size(); ++i) {
		if (array[i-1]->getResolution()!=array[i]->getResolution()) {
			return false;
		}
	} 
	return true;
}
