#pragma once
#include <chrono>
#include <string>
#include <iostream>
#include <utility>


struct Loading {
	Loading(int stages):stagesNumber(stages) {
		reset();
	}

	Loading(const Loading&) = delete;

	~Loading() = default;

	float getProgress() const {
		return currentStage / stagesNumber;
	}

	void reset() {
		
		currentStage = 0;
		start = std::chrono::system_clock::now();
	}

	void reset(std::string _msg) {
		msg = std::move(_msg);
		reset();
	}

	void inc() {
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);
		std::cout << currentStage << " completed (" << msg.c_str() << ") at " << duration.count() << " ms" << std::endl;
		start = std::chrono::system_clock::now();
		++currentStage;
	}

	void inc(std::string _msg) {
		inc();
		msg = std::move(_msg);
	}

	int currentStage;
private:
	float stagesNumber;
	std::string msg = "";
	std::chrono::system_clock::time_point start;
};
