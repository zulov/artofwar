#pragma once

#include <iostream>

inline void PRINT() {
	std::cout << std::endl;
}

template <typename T, typename... Args>
void PRINT(T first, Args... args) {
	std::cout << first << " "; 
	PRINT(args...);
}

template <typename T>
void PRINT_S(std::vector<std::vector<T>> vs) {
	for (auto v : vs) {
		std::cout << v.size() << ";";
	}
	std::cout << std::endl;
}