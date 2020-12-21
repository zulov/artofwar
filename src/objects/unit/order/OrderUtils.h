#pragma once

template <typename T>
void removeExpired(std::vector<T*>& orders) {
	orders.erase(std::remove_if(orders.begin(), orders.end(),
	                            [](T* uo) {
		                            const bool expired = uo->expired();
		                            if (expired) {
			                            delete uo;
		                            }
		                            return expired;
	                            }),
	             orders.end());
}
