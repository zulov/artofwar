#pragma once
#include "env/influence/map/InfluenceField.h"

template<std::size_t N>
inline bool validSizes(std::array<InfluenceField*, N> &array) {
	for (int i = 1; i < array.size(); ++i) {
		if (array[i-1]->getResolution()!=array[i]->getResolution()) {
			return false;
		}
	} 
	return true;
}
