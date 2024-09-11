#pragma once

#include <iostream>

inline void PRINT() {
	std::cout << std::endl;
}

template <typename T, typename... Args>
void PRINT(T first, Args... args) {
	std::cout << first << " "; 
	print(args...);
}