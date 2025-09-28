#pragma once
#include <chrono>
#include <string>
#include <iostream>
#include <utility>


struct Loading {
	Loading(unsigned char stages, bool ifPrint = true): stagesNumber(stages), ifPrint(ifPrint) {
		reset();
	}

	Loading(const Loading&) = delete;

	~Loading() = default;

	float getProgress() const {
		return currentStage / (float)stagesNumber;
	}

	void reset() {
		currentStage = 0;
		start = std::chrono::system_clock::now();
	}

	void reset(std::string _msg) {
		msg = std::move(_msg);
		reset();
	}

	void inc(std::string _msg) {
		if (ifPrint) {
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);
			std::cout << (int)currentStage << " completed (" << msg.c_str() << ") at " << duration.count() << " ms" << std::endl;
			start = std::chrono::system_clock::now();
			msg = std::move(_msg);
		}

		inc();
	}

	void inc() {
		++currentStage;
	}

	unsigned char currentStage;
	const unsigned char stagesNumber;
private:
	const bool ifPrint;
	std::string msg = "";
	std::chrono::system_clock::time_point start;
};
