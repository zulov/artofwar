#pragma once
#include <chrono>

struct loading
{
	loading() {
		stagesNumber = 1;
		currentStage = 0;
		sth = nullptr;
		start = std::chrono::system_clock::now();
	}

	~loading(){}

	float getProgres() {
		return currentStage / stagesNumber;
	}

	void reset(int stages) {
		stagesNumber = stages;
		currentStage = 0;
		sth = nullptr;
	}

	void inc() {
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);
		std::cout << currentStage << " complated at " << duration.count() << std::endl;
		start = std::chrono::system_clock::now();
		++currentStage;
	}

	float stagesNumber;
	int currentStage;
	void* sth;
	chrono::system_clock::time_point start;
};
