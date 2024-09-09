#include "DebugUtils.h"

#include <iostream>
int counterX = 0;
int counterY = 0;

void COUNT_X() {
	++counterX;
}
void COUNT_Y() {
	++counterY;
}

void PRINT_X2Y(){
	if(counterX+ counterY>0) {
		int sum = (counterX + counterY) / 100;
		std::cout << "X: " << counterX << " Y: " << counterY << "| X-Y " << (counterX / sum) << "%-" << (counterY / sum) <<"%"<< std::endl;
	}
}