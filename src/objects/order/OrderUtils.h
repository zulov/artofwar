#pragma once
#include "UnitOrder.h"

inline void removeExpired(std::vector<UnitOrder*>& orders) {
	orders.erase(std::remove_if(orders.begin(), orders.end(),
	                            [](UnitOrder* uo) {
		                            const bool expired = uo->expired();
		                            if (expired) { delete uo; }
		                            return expired;
	                            }),
	             orders.end());
}
