#pragma once
#include "env/influence/map/InfluenceMapFloat.h"

template<std::size_t N>
inline bool validSizes(std::array<InfluenceMapFloat*, N> &array) {
	for (int i = 1; i < array.size(); ++i) {
		if (array[i-1]->getResolution()!=array[i]->getResolution()) {
			return false;
		}
	} 
	return true;
}
