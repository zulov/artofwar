#pragma once
#include <iostream>

void COUNT_X();
void COUNT_Y();
void PRINT_X2Y();

void PRINT();

template <typename T, typename... Args>
void PRINT(T first, Args... args) {
	std::cout << first << " "; 
	print(args...);
}